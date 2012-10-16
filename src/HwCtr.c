#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <asm/msr.h>

#include "HwCtr.h"

#if defined(CONFIG_MODVERSIONS)
#define MODVERSIONS
// #include <linux/modversions.h>
#endif

                            // "fops" contains the jump table
			    // used to access individual
			    // functions within this module.
static struct file_operations fops = {
    .owner   =  THIS_MODULE,
    .llseek  =  d_llseek,
    .read    =  d_read,
    .write   =  d_write,
    .open    =  d_open,
    .release =  d_close,
    .ioctl   =  d_ioctl,
};

			    // "major_dev" contains (after init)
			    // the major device number to be used
			    // to access this module.
//static int major_dev = 0;

			    // "beginning_of_time" represents
			    // the real time stamp value when
			    // counting began.
static long beginning_of_time = 0L;

			    // "counters" represents the state
			    // of the counters currently in use.
static struct {
    long  start;	    // rtc time at start of latest measurement
    short type;		    // type of event being counted
} counters [ CONFIG_NR_CPUS ][ MAX_COUNTERS ];

			    // "events" represents the 
			    // accumulation of events over time
			    // as measured by the counters.
static struct {
    long  count;	    // number of events recorded
    long  cycles;	    // events were recorded over this many CPU cycles
    short type;	            // type of event
} events [ CONFIG_NR_CPUS ][ MAX_COUNTERS ];

                           
                             // "state" is the mode that the module is in 
                             // for receiving data from and sending data 
                             // to Arum
int control_state = NO_STATE;

int control_count = 0;       // "control_count" is the current count of
                             // ops completed.

int control_reqOps = 0;      // "contol_reqOps" is the total number of
                             // ops to be completed.

                             //table of all possilbe events
struct possEvent * possibleEvents;
int  possEvtTableSz;
                             //table of all counters
struct reg * allCounters;             
int allCountersTableSz;
                             //The following tables are architecture
                             //dependent. Arum currently supports
                             //two non-counter types of registers 
                             //that are used in performance
                             //counting: Event selection (escr) and 
                             //Counter Configuration (cccr) registers.
struct nonCounterReg * escrTable;     
int escrTableSz;

struct nonCounterReg * cccrTable;
int cccrTableSz;

unsigned CCCR_ADDR = 0;
unsigned CCCR_CONFIG = 0;

int hwctr_major		= HWCTR_MAJOR; 
int hwctr_minor		= 0;

module_param(hwctr_major, int, S_IRUGO);
module_param(hwctr_minor, int, S_IRUGO);


static int 
setup_cdev (struct hwctr_dev *dev)
{
   int err = 0;
   int devno = MKDEV (hwctr_major, hwctr_minor);
   cdev_init(&dev->hwctr_cdev, &fops);
   dev->hwctr_cdev.owner = THIS_MODULE;
   dev->hwctr_cdev.ops = &fops;
   err = cdev_add(&dev->hwctr_cdev, devno, 1);
   if (err < 0){
      printk (KERN_WARNING DEVNAME ": cdev add failed\n");
   }
   return err;
}

/*************
static int
HwCtr_init( void )
{
				// find the device major number to use
    int rv = register_chrdev( major_dev, DEVNAME, &fops );
    unsigned long cr4 = (unsigned long) get_cr4();
    if (rv < 0) {
	printk( KERN_INFO DEVNAME ": Registration failed.\n" );
	return rv;
    }
    if (major_dev == 0) major_dev = rv;

				// record the device major number so 
				// an appropriate device can be created
    printk( KERN_INFO DEVNAME ": Registered at major number %d.\n", major_dev );

    printk( KERN_INFO DEVNAME ": cr4 = 0x%16.16lx\n", cr4 );

    return 0;
}
******************/
static int
HwCtr_init(void)
{
    int rv;
    dev_t dev = 0;
    unsigned long cr4;

    if (hwctr_major) {
        dev = MKDEV(hwctr_major, hwctr_minor);
        rv = register_chrdev_region (dev, HWCTR_NR_DEVS, DEVNAME);
    } else {
        rv =alloc_chrdev_region(&dev, hwctr_minor, HWCTR_NR_DEVS, DEVNAME);
        hwctr_major = MAJOR(dev);
    }    

    cr4 = (unsigned long) get_cr4();
    if (rv < 0) {
        printk( KERN_INFO DEVNAME ": Registration failed.\n" );
        return rv;
    }
                                // record the device major number so
                                // an appropriate device can be created
    printk( KERN_INFO DEVNAME ": Registered at major number %d.\n", 
            hwctr_major );
    printk( KERN_INFO DEVNAME ": cr4 = 0x%16.16lx\n", cr4 );

                                //initialize hwctr_dev
    memset (&hdev, 0, HWCTR_NR_DEVS * sizeof(struct hwctr_dev));
    rv = 0;
    rv = setup_cdev (&hdev);
    if (rv < 0) {
       return rv;
    }
    return 0;
}


static void
HwCtr_finish( void )
{
    dev_t devno = MKDEV(hwctr_major, hwctr_minor);

                                // free up the device major number
                                // so some other module can use it.
    cdev_del (&hdev.hwctr_cdev);
    unregister_chrdev_region( devno, HWCTR_NR_DEVS);
    printk( KERN_INFO DEVNAME ": Deregistration (major=%d) succeeded.\n", 
            hwctr_major );
}


static loff_t
d_llseek( struct file *fp, loff_t off, int whence )
{
    //struct hwctr_dev * dev = fp->private_data;
    loff_t newpos;

    //always set seek to begining
    newpos = 0;
    fp->f_pos = newpos;   
    return newpos;
}


/*
  Return hardware counter data to the userspace buffer, buf.
  If there is an error, return a negative number; otherwise
  return the number of results returned.    
*/
static ssize_t
d_read( struct file *filp, char *buf, size_t count, loff_t *whence )
{

    int i, rv = 0;
    //struct hwctr_dev * dev = filp->private_data;
    struct configStruct cBuf = {4, 5, 6};
    char * kbuf;
    unsigned long perfResults = 0x0;


    /**
     do some debug printing
    **/
    printk (KERN_NOTICE DEVNAME ": d_read: entry - curr state is %d\n", control_state);
    if (possibleEvents != NULL) {
       for (i = 0; i < possEvtTableSz; i++) {
          printk (KERN_NOTICE DEVNAME ": d_read: possibleEvent[%d]:%s\n",
                  i, possibleEvents[i].name);
       }
    }
    if (allCounters != NULL) {
       for (i=0; i < allCountersTableSz; i++) {
          if (allCounters[i].addr != 0xffffffffffffffff) {
             printk (KERN_NOTICE DEVNAME ": d_read: allCounters[%d]:%s-0x%8.8lx\n",
                  i, allCounters[i].name, allCounters[i].addr);
          }
       }
    }
    if (escrTable != NULL) {
       for (i=0; i<escrTableSz; i++){
          printk( KERN_NOTICE DEVNAME ": d_read: escrs[%d]:0x%8.8lx--%s--0x%16.16lx\n",
               i, escrTable[i].addr, escrTable[i].name,
               escrTable[i].defaultConfig );
       }
    } 
    if (cccrTable != NULL) {
       for (i=0; i< cccrTableSz; i++) {
          printk( KERN_NOTICE DEVNAME ": d_read: cccrs[%d]:0x%8.8lx--%s--0x%16.16lx\n",
               i, cccrTable[i].addr, cccrTable[i].name,
               cccrTable[i].defaultConfig );
       } 
    }
    /* To read counters:
    1. Read counters 
    */

    perfResults = read_counter (0x30c);
    printk (KERN_NOTICE DEVNAME ": d_read: counter == 0x%16.16lx\n", 
            perfResults);

    /*
    2. Halt counting 
    */
    stop_counting (); 

    kbuf = kmalloc(count * sizeof(char *), GFP_KERNEL);
    if (!kbuf) {
       return -ENOMEM;
    }
    memset (kbuf, '\0', count * sizeof(char *));
    strcpy (kbuf, "01   1234567, 01   123654");
    //if (copy_to_user(buf, kbuf, count)) {
    //   printk (KERN_NOTICE DEVNAME ": d_read: copy_to_user failed\n");
    //   return -EFAULT;
    //} else {
    //   rv = strlen(kbuf);
    //}

    if (copy_to_user(buf, &cBuf, sizeof(struct configStruct))) {
       printk (KERN_NOTICE DEVNAME ": d_read: copy_to_user failed\n");
       return -EFAULT; 
    } else {
       rv = sizeof(struct configStruct);
    }
    *whence += rv; 

   
    return rv;
}

static ssize_t
d_write( struct file *filp, const char *buf, size_t count, loff_t *whence )
{
    int i;
    unsigned escr_msr = 0, cccr_msr = 0, pmc_msr = 0, escr_val = 0, cccr_val;

    union counterConfig ucc;

    /* escr config vars */
    __u32 escr_usrFlag, escr_osFlag, escr_tagFlag;
    __u32 escr_tagVal, escr_evtMask, escr_evtSel;

    /* cccr config var */
    __u32 cccr_enableFlag, cccr_escrSel, cccr_compareFlag;
    __u32 cccr_complementFlag, cccr_threshold, cccr_edgeFlag;
    __u32 cccr_forceOvfFlag, cccr_OvfPmiFlag, cccr_cascadeFlag;
    __u32 cccr_ovfFlag;

    struct hwctr_dev * dev = filp->private_data;

    printk (KERN_NOTICE DEVNAME ": d_write: entry - curr state is %d\n", control_state);

    //
    // The way to interpret what is in the user buffer depends on the 
    // control-state.  If current state is NO_STATE, then the buffer contains 
    // a control struct, indicating what operation to start next, and how many 
    // to do.  If current state is INIT, then the module is in the process of
    // initializing global data structures.  If the current state is CONFIG, 
    // then the module is in the process of configuring registers for counting.
    // If the current state is COUNTING, then counting has begun.  The global
    // variables "control_state" and "control_count" are used to check and set
    // state and to track how many operations have been completed.
    //
    switch (control_state) {
       case NO_STATE:
          //Read in a control structure.
          //set state, count, and reqOps.
          //if success return 1, otherwise negative number.
          if (copy_from_user(&ucc, buf, count)) {
             printk (KERN_NOTICE DEVNAME ": d_write: copy_from_user failed\n");
             return -EFAULT;
          }
          printk( KERN_NOTICE DEVNAME ": d_write counter NO_STATE data from user: flag:%d count:%d\n",
                  ucc.control.flag, ucc.control.count );
          control_state = ucc.control.flag;
          control_reqOps = ucc.control.count;
          control_count = 0;
          printk( KERN_NOTICE DEVNAME ": d_write counter change state to:%d count:%d\n",
                  control_state, control_count );
          *whence += count;
          return 1;
          break;

       case INIT_EVENTS:
          //Read in an event structure.
          //if count == 0, then create the possibleEvents table with size
          //   reqOps.
          //update appropriate position in possbileEvents table.
          //increment count.
          //if count == reqOps, then this was last one -- change
          //  state back to NO_STATE and return 1.
          //if count < reqOps, then return 1.
          //Return negative number on error.
          memset (&ucc, '\0', sizeof (union counterConfig));
          if (control_count == 0) {
             possibleEvents = 
                (struct possEvent *) kmalloc
                (control_reqOps * sizeof(struct possEvent), GFP_KERNEL);
             if (possibleEvents == NULL) {
                printk( KERN_NOTICE DEVNAME ": d_write: can't allocate memory\n");
                return -ENOMEM;
             }
             possEvtTableSz = control_reqOps;
          }
          if (copy_from_user(&ucc, buf, count)) {
             printk (KERN_NOTICE DEVNAME ": d_write: copy_from_user failed\n");
             return -EFAULT;
          }
          memset(possibleEvents[ucc.event.index].name, '\0', EVT_NAME_LEN);
          strcpy(possibleEvents[ucc.event.index].name, ucc.event.name);
          for (i=0; i<MAX_CTRS_PER_EVT; i++) {
             possibleEvents[ucc.event.index].ctrs[i] = ucc.event.ctrs[i]; 
          }
          //printk( KERN_NOTICE DEVNAME ": d_write INIT_EVENTS data from user: %d %s\n",
          //        ucc.event.index, ucc.event.name );
          printk( KERN_NOTICE DEVNAME ": d_write INIT_EVENTS new possEvt data: index:%d name:%s ",
                  ucc.event.index, possibleEvents[ucc.event.index].name );
          printk( "counters: ");
          for (i=0; i < MAX_CTRS_PER_EVT; i++){
             if (possibleEvents[ucc.event.index].ctrs[i] == -1) {
                break;
             }
             printk( "%d ", possibleEvents[ucc.event.index].ctrs[i]);
          }
          printk ( "\n");

          control_count ++;
          if (control_count == control_reqOps) {
             control_state = NO_STATE;
             *whence += count;
             return 1;
          }
          *whence += count;
          return 1;
          break;

       case INIT_CTRS:
          //Read in a counter structure.
          //if count == 0, then create the counters table with size
          //   reqOps.
          //update appropriate position in counters table.
          //increment count.
          //if count == reqOps, then this was last one -- change
          //  state back to NO_STATE and return 1.
          //if count < reqOps, then return 1.
          //Return negative number on error.
          memset (&ucc, '\0', sizeof (union counterConfig));
          if (control_count == 0) {
             allCounters =
                (struct reg *) kmalloc
                (control_reqOps * sizeof(struct reg), GFP_KERNEL);
             if (allCounters == NULL) {
                printk( KERN_NOTICE DEVNAME ": d_write: can't allocate memory\n");
                return -ENOMEM;
             }
             allCountersTableSz = control_reqOps;
             //ititialize table
             for (i=0; i<allCountersTableSz; i++){
                allCounters[i].addr = 0xffffffffffffffff;
                memset(allCounters[i].name, '\0', REG_NAME_LEN);
             }
          }
          if (copy_from_user(&ucc, buf, count)) {
             printk (KERN_NOTICE DEVNAME ": d_write: copy_from_user failed\n");
             return -EFAULT;
          }
          //printk( KERN_NOTICE DEVNAME ": d_write INIT_CTRS data from user: %d %s\n",
          //        ucc.counter.index, ucc.counter.name );
          if (ucc.counter.index != -1) {
             allCounters[ucc.counter.index].addr = ucc.counter.addr;
             memset(allCounters[ucc.counter.index].name, '\0', REG_NAME_LEN);
             strcpy(allCounters[ucc.counter.index].name, ucc.counter.name);
             printk( KERN_NOTICE DEVNAME ": d_write INIT_CTRS index:%d name:%s addr:0x%8.8lx\n",
                  ucc.counter.index, allCounters[ucc.counter.index].name,
                  allCounters[ucc.counter.index].addr );
          }
          control_count ++;
          if (control_count == control_reqOps) {
             control_state = NO_STATE;
             *whence += count;
             return 1;
          }
          *whence += count;
          return 1;

       case INIT_ESCRS:
          //Read in a non-counter escr structure.
          //if count == 0, then create the escrs table with size
          //   reqOps.
          //update appropriate position in the escrs table.
          //increment count.
          //if count == reqOps, then this was last one -- change
          //  state back to NO_STATE and return 1.
          //if count < reqOps, then return 1.
          //Return negative number on error.
          memset (&ucc, '\0', sizeof (union counterConfig));
          if (control_count == 0) {
             escrTable =
                (struct nonCounterReg *) kmalloc
                (control_reqOps * sizeof(struct nonCounterReg), GFP_KERNEL);
             if (escrTable == NULL) {
                printk( KERN_NOTICE DEVNAME ": d_write: can't allocate memory\n");
                return -ENOMEM;
             }
             escrTableSz = control_reqOps;
             //ititialize table
             for (i=0; i<escrTableSz; i++){
                escrTable[i].addr = 0xffffffffffffffff;
                memset(escrTable[i].name, '\0', REG_NAME_LEN);
                escrTable[i].defaultConfig = 0;
                escrTable[i].inUse = UNUSED; 
             }
          }
                                         //read in data from user
          if (copy_from_user(&ucc, buf, count)) {
             printk (KERN_NOTICE DEVNAME ": d_write: copy_from_user failed\n");
             return -EFAULT;
          }
          //printk( KERN_NOTICE DEVNAME ": d_write INIT_ESCRS data from user: 0x%8.8lx %s 0x%16.16lx\n",
          //        ucc.escr.addr, ucc.escr.name, ucc.escr.defaultConfig );
          escrTable[control_count].addr = ucc.escr.addr;
          memset(escrTable[control_count].name, '\0', REG_NAME_LEN);
          strcpy(escrTable[control_count].name, ucc.escr.name);
          printk( KERN_NOTICE DEVNAME ": d_write INIT_ESCRS addr:0x%8.8lx name:%s default:0x%16.16lx\n",
               escrTable[control_count].addr, escrTable[control_count].name,
               escrTable[control_count].defaultConfig );
          control_count ++;
          if (control_count == control_reqOps) {
             control_state = NO_STATE;
             *whence += count;
             return 1;
          }
          *whence += count;
          return 1;

       case INIT_CCCRS:
          //Read in a non-counter cccr structure.
          //if count == 0, then create the cccrs table with size
          //   reqOps.
          //update appropriate position in the cccrs table.
          //increment count.
          //if count == reqOps, then this was last one -- change
          //  state back to NO_STATE and return 1.
          //if count < reqOps, then return 1.
          //Return negative number on error.

          memset (&ucc, '\0', sizeof (union counterConfig));
          if (control_count == 0) {
             cccrTable =
                (struct nonCounterReg *) kmalloc
                (control_reqOps * sizeof(struct nonCounterReg), GFP_KERNEL);
             if (cccrTable == NULL) {
                printk( KERN_NOTICE DEVNAME ": d_write: can't allocate memory\n");
                return -ENOMEM;
             }
             cccrTableSz = control_reqOps;
             //ititialize table
             for (i=0; i<cccrTableSz; i++){
                cccrTable[i].addr = 0xffffffffffffffff;
                memset(cccrTable[i].name, '\0', REG_NAME_LEN);
                cccrTable[i].defaultConfig = 0;
                cccrTable[i].inUse = UNUSED;
             }
          }
                                         //read in data from user
          if (copy_from_user(&ucc, buf, count)) {
             printk (KERN_NOTICE DEVNAME ": d_write: copy_from_user failed\n");
             return -EFAULT;
          }
          //printk( KERN_NOTICE DEVNAME ": d_write INIT_CCCRS data from user: 0x%8.8lx %s 0x%16.16lx\n",
          //        ucc.cccr.addr, ucc.cccr.name, ucc.cccr.defaultConfig );
          cccrTable[control_count].addr = ucc.cccr.addr;
          memset(cccrTable[control_count].name, '\0', REG_NAME_LEN);
          strcpy(cccrTable[control_count].name, ucc.cccr.name);
          printk( KERN_NOTICE DEVNAME ": d_write INIT_CCCRS addr:0x%8.8lx name:%s default:0x%16.16lx\n",
               cccrTable[control_count].addr, cccrTable[control_count].name,
               cccrTable[control_count].defaultConfig );
          control_count ++;
          if (control_count == control_reqOps) {
             control_state = NO_STATE;
             *whence += count;
             return 1;
          }
          *whence += count;
          return 1;

       case CONFIG_EVENTS:
          //Read in a configEvent structure
          //look up event in curent events table:
          //   IF event is listed, return an error
          //   IF not listed, check to see if the 
          //     requested counter is available, by
          //     checking the current counters table.
          //     IF counter is not available, return an error
          //     If counter is available, check if the non-
          //        counter registers are available.
          //        IF all specified are available, then
          //           configure the non-counter registers,
          //           update the current events table, and
          //           update the current counters table.
          //        IF one or more are not available, return an error. 
          //Increment count.
          //if count == reqOps, then this was last one -- change
          //  state back to NO_STATE and return 1.
          //if count < reqOps, then return 1.
          //Return negative number on error (value indicats error type).
          memset (&ucc, '\0', sizeof (union counterConfig));
          if (copy_from_user(&ucc, buf, count)) {
             printk (KERN_NOTICE DEVNAME ": d_write: copy_from_user failed\n");
             return -EFAULT;
          }
          printk( KERN_NOTICE DEVNAME ": d_write CONFIG_EVENTS data from user: %d %d %d %d\n",
                  ucc.config.eIndex, ucc.config.cNumber, ucc.config.escrFlag,
                  ucc.config.cccrFlag);
          printk( "   0x%8.8lx 0x%16.16lx 0x%8.8lx 0x%16.16lx\n",
                  ucc.config.escrAddr, ucc.config.escrConfig, 
                  ucc.config.cccrAddr, ucc.config.cccrConfig );
          control_count ++;
          if (control_count == control_reqOps) {
             control_state = NO_STATE;
             *whence += count;
             return 1;
          }
          *whence += count;
          return 1;
          break;

       case START_COUNTERS:
         //Read in a startCounting structure
         //For all counters listed in current counters table,
         //  reset contents to 0.
         //For all, non-counter registers in use, flip the appropriate
         //  start counting bit.
         //Change to COUNTING state
         //Return 1.
         break;

       case COUNTING:
          //If in the counting state, a configure register
          //is expected, with the configuration for starting
          //counting.  d_read handles moving out of this state.
          break;

       default:
          break;
    }

    //Set up registers for counting 
    // 1. What is the event?  example event is instr_completed
    //      a.  ESCRs it can use
    //          MSR_CRU_ESCR0, MSR_CRU_ESCR1
    //      b.  PMC numbers for each ESCR
    //          (12, 13, 16), (14, 15, 17)
    //      c.  ESCR Event selection code
    //          07h
    //      d.  ESCR Event Mask 
    //          0:NBOGUS, 1:BOGUS
    //      e.  CCCR select
    //          04h

    // 2. Choose a performance counter (each maps to a CCCR) 
    //          name            #  addr         name         addr 
    //       a. MSR_IQ_COUNTER0 12 30Ch <---->  MSR_IQ_CCCR0 36Ch
    //         with ESCR MSR_CRU_ESCR0 4 3B8h
    
    pmc_msr  = 0x30c;
    escr_msr = 0x3b8;
    reset_escr(escr_msr);   //zero out escr

    // 3. Setup ESCR [31:0] using WRMSR instruction
    //        flag/field  bit position  value
    //        usr            2            1
    //         os            3            1
    //        tag enable     4            0
    //        tag val      [8:5]          00h
    //        evt mask     [24:9]         03h
    //        evt sel      [30:25]        07h 
    //
    escr_usrFlag = 0x1 << 2;    // turn on
    escr_osFlag  = 0x1 << 3;    // turn on
    escr_tagFlag = 0x0 << 4;    // turn off
    escr_tagVal  = 0x0 << 5;    // zero value
    escr_evtMask = 0x3 << 9;    // count bogus and non-bogus
    escr_evtSel  = 0x7 << 25;   // 07h is code to  use
    escr_val = (escr_usrFlag | escr_osFlag | escr_tagFlag | 
                escr_tagVal | escr_evtMask | escr_evtSel); 

    printk (KERN_INFO DEVNAME ": d_write: escr bit config  == 0x%8.8x\n", escr_val);
    setup_escr(escr_msr, escr_val);

    // 4. Setup CCCR with WRMSR instruction
    //        flag/field  bit position  value
    //        enable        12            1
    //        ESCR sel     [15:13]        04h (this is the CCCR select that maps
    //                                         to the ESCR)
    //        compare       18            0
    //        complement    19            0
    //        threshold    [23:20]        00h 
    //        edge          24            0
    //        FORCE_OVF     25            0
    //       OVF_PMI       26            0
    //        cascade       30            0
    //        ovf           31            0 (initialized - this flag changes to 1 on ovf.
    //                                       Software must explicitly clear flag)
    // 

    cccr_msr = 0x36c;
    reset_cccr(cccr_msr);    //zero out cccr

    cccr_enableFlag     = 0x0 << 12;    /*do not enable yet*/
    cccr_escrSel        = 0x4 << 13;
    cccr_compareFlag    = 0x0 << 18;
    cccr_complementFlag = 0x0 << 19;
    cccr_threshold      = 0x0 << 20;
    cccr_edgeFlag       = 0x0 << 24;
    cccr_forceOvfFlag   = 0x0 << 25;
    cccr_OvfPmiFlag     = 0x0 << 26;
    cccr_cascadeFlag    = 0x0 << 30;
    cccr_ovfFlag        = 0x0 << 31;

    cccr_val =  ( cccr_enableFlag   | cccr_escrSel        |
                  cccr_compareFlag  | cccr_complementFlag |
                  cccr_threshold    | cccr_edgeFlag       |
                  cccr_forceOvfFlag | cccr_OvfPmiFlag     |
                  cccr_cascadeFlag  | cccr_ovfFlag 
                );
    printk (KERN_INFO DEVNAME ": cccr bit config  == 0x%8.8x\n", cccr_val);
    setup_cccr (cccr_msr, cccr_val);

    /*5. Start counting*/
    reset_pmc(pmc_msr);               //zero out counter
    start_counting ();           


    *whence += count;
    printk (KERN_INFO DEVNAME ": returning %d\n", (int) count);
    return count;
}

				// "ctr_in_use" is used to ensure
				// only a single process may use
				// this module at any given time.
static volatile int ctr_in_use = UNUSED;

				// "use_lock" is used to protect
				// "ctr_in_use" from concurrent
				// access.
static spinlock_t use_lock = SPIN_LOCK_UNLOCKED; 

static int
d_open( struct inode *inode, struct file *filp )
{
				// only one user is allowed to 
				// use this module at a time...
				// so set the "in use" flag
    int rv = 0;
    struct hwctr_dev * dev;
    dev = container_of (inode->i_cdev, struct hwctr_dev, hwctr_cdev);
    filp->private_data = dev;
    
    spin_lock(&use_lock);
    if (ctr_in_use == UNUSED) {
	ctr_in_use = IN_USE;
        control_state = NO_STATE;
        control_count = 0;
        control_reqOps = 0;
        printk (KERN_WARNING DEVNAME ": d_open - got lock\n");
    } else {
	rv = -EBUSY;
        return rv;
    }
    spin_unlock(&use_lock);

    
    return rv;
}

static int
d_close( struct inode *inode, struct file *filp )
{
				// we're done with the module so...
				// clear the "in use" flag
    spin_lock(&use_lock);
    ctr_in_use = UNUSED;
    spin_unlock(&use_lock);
    control_state = NO_STATE;
    control_count = 0;
    control_reqOps = 0;
    return 0;
}

static int
d_ioctl(struct inode *i, struct file *f, unsigned int x, unsigned long y)
{
    return 0L;
}


/*
  Uses inline asm to write to a msr register using wrmsr instruction
  inputs: msr - addr of msr to write to 
          val - bit configuration that is to be written to the low-order
          32 bits of the msr 
  Description: zero out edx, ecx is loaded with msr, eax is loaded
               with val. wrmsr loads the contents of edx and eax into 
               the MSR specified by address in ecx.
*/
static void
do_wrmsr (unsigned msr, unsigned val) {
   __asm__ __volatile__
   (
      "wrmsr"
      :
      : "c"(msr), "a" (val), "d" (0)
   );
   printk (KERN_INFO DEVNAME ": do_wrmsr - val == 0x%8.8x\tmsr == 0x%8.8x\n", 
           val, msr);
}


/*
  Sets up the ESCR msr for counting. This is a wrapper to do_wrmsr
  inputs: addr of msr to use for the ESCR
          bit configuration that is to be written to the low-order
          32 bits of the ESCR (the high-order 32 bits are reserved)
*/
static void
setup_escr (unsigned msrAddr, unsigned lowOrderConfig)
{
   do_wrmsr (msrAddr, lowOrderConfig);
}


/*
  Sets up the CCCR msr for performance counting.  This is a wrapper to
  do_wrmsr.
  inputs: cccrAddr - addr of the msr to use for the CCCR
          lowOrderConfig - the bit configuration that is to be written
                     to the low order 32 bits of the CCCR.
*/
static void
setup_cccr (unsigned cccrAddr, unsigned lowOrderConfig)
{
  CCCR_ADDR = cccrAddr;
  CCCR_CONFIG = lowOrderConfig;
  do_wrmsr (CCCR_ADDR, CCCR_CONFIG);
}

/*
  Reads the performance counter specified by the pmcAddr.
  Returns the 40-bit value as an unsigned long
*/
static unsigned long
read_counter (unsigned pmcAddr)
{
   unsigned long data = 0x0;
   do_rdmsr(pmcAddr, &data);
   printk(KERN_INFO DEVNAME ": read_counter - data == 0x%16.16lx\n",
           data);
   return data;
}

/*
  Reads msr specified by msr; stores result in data.
*/
static void
do_rdmsr (unsigned msr, unsigned long * data)
{
   unsigned long lhigh = 0x0;
   unsigned long llow  = 0x0;
   __u32 high = 0;   /*for edx*/
   __u32 low =  0;    /*for eax*/

/***** for debugging ****
   high = 0xffffffff;
   low  = 0xaaaaaaaa;
***********************/

   __asm__ __volatile__
   (
      "rdmsr;"
      :"=d"(high), "=a"(low)
      :"c"(msr)
   );

   lhigh = (lhigh | high);
   lhigh = lhigh << 32;
   llow = low;
   *data = (lhigh | llow);

   printk (KERN_INFO DEVNAME ": do_rdmsr - data == 0x%16.16lx\n", (unsigned long) *data);

}

/*
  This sets the enable bit in the CCCR.  Once set, counting begins
*/
static void
start_counting (void)
{
   __u32 turnOnBit = 0x0;
   turnOnBit = 0x1 << 12;

   CCCR_CONFIG = (CCCR_CONFIG | turnOnBit); 
   printk (KERN_INFO DEVNAME ": start_counting cccr == 0x%8.8x\n",
           CCCR_CONFIG);
   do_wrmsr (CCCR_ADDR, CCCR_CONFIG);
}

/*
  This clears the enable bit in the CCCR. Once cleared, counting halts.
*/
static void
stop_counting (void)
{
   __u32 turnOffBit = 0xffffefff;

   CCCR_CONFIG = (CCCR_CONFIG & turnOffBit);
   printk (KERN_INFO DEVNAME ": stop_counting cccr == 0x%8.8x\n",
           CCCR_CONFIG);
   do_wrmsr (CCCR_ADDR, CCCR_CONFIG);
}

/*
  zero out the ESCR specified by msrAddr, the address of the ESCR
*/
static void
reset_escr (unsigned msrAddr)
{
   //bits 0,1,31-63 are reserved for Intel ESCRs for NetBurst Microarch.
   //mask for the low 32 bits is 80000003h.
   unsigned long data = 0x0, sanityCheck = 0x0;
   __u32 currLow32Escr = 0x0;
   __u32 escrMask = 0x80000003;
   __u32 newEscrVal = 0x0;

   do_rdmsr(msrAddr, &data);
   printk (KERN_INFO DEVNAME ": reset_escr: escr @ 0x%8.8x contains 0x%16.16lx\n",
           msrAddr, data);

   currLow32Escr = (__u32)data;
   printk (KERN_INFO DEVNAME ": reset_escr: curLow32Escr contains 0x%8.8x\n",
           currLow32Escr); 
   newEscrVal = currLow32Escr & escrMask;
   do_wrmsr(msrAddr, newEscrVal);

   /**** some debug checking *********
   do_rdmsr(msrAddr, &sanityCheck);
   printk (KERN_INFO DEVNAME ": reset_escr: escr @ 0x%8.8x now contains 0x%16.16lx\n",
           msrAddr, sanityCheck);
   ***********************************/
}

static void
reset_cccr (unsigned msrAddr)
{
// The only difference from reset_escr is the mask used.
// Reserved bits for Intel NetBurst Microarch: 0-11,16,17,27-29,32-63
// mask for the low 32 bits is 38030FFFh.

   unsigned long data = 0x0, sanityCheck = 0x0;
   __u32 currLow32Cccr = 0x0;
   __u32 cccrMask = 0x38030fff;
   __u32 newCccrVal = 0x0;

   do_rdmsr(msrAddr, &data);
   printk (KERN_INFO DEVNAME ": reset_cccr: cccr @ 0x%8.8x contains 0x%16.16lx\n",
           msrAddr, data);

   currLow32Cccr = (__u32)data;
   printk (KERN_INFO DEVNAME ": reset_cccr: curLow32Cccr contains 0x%8.8x\n",
           currLow32Cccr);
   newCccrVal = currLow32Cccr & cccrMask;
   do_wrmsr(msrAddr, newCccrVal);

   /**** some debug checking *********/
   do_rdmsr(msrAddr, &sanityCheck);
   printk (KERN_INFO DEVNAME ": reset_cccr: cccr @ 0x%8.8x now contains 0x%16.16lx\n",
           msrAddr, sanityCheck);
   /***********************************/
}

static void
reset_pmc(unsigned pmcAddr)
{
// This reset method zeros out the lower 40bits of the PMC. 
// This method calls wrmsr with 0 in EAX and a special value
// in EDX (most likely zero).
// Reserved bits for Intel Net Burst Microarch: 40-63.
// Mask for the high 32 bits of the PMC is FFFFFF00h.

   unsigned long data = 0x0, sanityCheck = 0x0;
   __u32 curHi32Pmc = 0x0;
   __u32 pmcMask = 0xffffff00;
   __u32 newHi32Pmc = 0x0;

   do_rdmsr(pmcAddr, &data);
   printk (KERN_INFO DEVNAME ": reset_pmc: pmc @ 0x%8.8x contains 0x%16.16lx\n",
           pmcAddr, data);

   curHi32Pmc =  data >> 32; 
   printk (KERN_INFO DEVNAME ": reset_pmc: curHi32Pmc contains 0x%8.8x\n",
           curHi32Pmc);
   newHi32Pmc = curHi32Pmc & pmcMask;

   __asm__ __volatile__
   (
      "wrmsr"
      :
      : "c"(pmcAddr), "a" (0), "d" (newHi32Pmc)
   );

   /**** some debug checking *********/
   do_rdmsr(pmcAddr, &sanityCheck);
   printk (KERN_INFO DEVNAME ": reset_pmc: pmc @ 0x%8.8x now contains 0x%16.16lx\n",
           pmcAddr, sanityCheck);
   /***********************************/
}

static unsigned long
get_cr4( void )
{
    /* bit 8 of CR4 is of interest to functionality of RDPMC. If this
       bit is set, then RDPMC can be executed at any privilege level.
       If the bit is clear, RDPMC can only be executed at CPL 0.
    */
    unsigned long cr4=0;

    __asm__ __volatile__
    (
	"mov %%cr4,%%rax;"
	"mov %%rax,%0;"
	:"=r"(cr4)
	:
	: "%rax" 
    );

    return cr4;
}

/*
  Turn the pce bit of cr4 (bit 8) on or off. If val is 0, clear the bit;
  if val is one, set the bit. 
  note: not implemented yet - this is the get_cr4 code.
*/
static void
set_cr4_pce ( int val )
{
   unsigned long cr4 = 0;
   __asm__ __volatile__
   (
        "mov %%cr4,%%rax;"
        "mov %%rax,%0;"
        :"=r"(cr4)
        :
        : "%rax"         
   );

}

/*
  Turn the tsd bit of cr4 (bit 2) on or off. If val is 0, clear the bit;
  if val is one, set the bit.
  note: not implemented yet.  This is the get_cr4 code
*/
static void
   set_cr4_tsd (int val)
{
   unsigned long cr4 = 0;
   __asm__ __volatile__
   (
        "mov %%cr4,%%rax;"
        "mov %%rax,%0;"
        :"=r"(cr4)
        :
        : "%rax"
   );
}

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Douglas M. Pase, IBM"); 
MODULE_DESCRIPTION("Application Resource Usage Monitor (Arum) -- Hardware counters"); 
MODULE_SUPPORTED_DEVICE(DEVNAME);

module_init(HwCtr_init);
module_exit(HwCtr_finish);



// CONFIG_NR_CPUS reflects the maximum number of CPUs the kernel can manage
// int kernel_thread(int (*fn)(void *), void * arg, unsigned long flags)
// cpu_online()
// smp_processor_id()
// interruptible_sleep_on_timeout()
// set_cpus_allowed()

