#if !defined(HwCtr_h)
#define HwCtr_h

#include <linux/fs.h>
#include <asm/types.h>
#include "defs.h"

/*************************************************
#define EVT_NAME_LEN	30
#define REG_NAME_LEN	20

#define INIT_EVENTS	1
#define INIT_CTRS	2
#define INIT_ESCRS	3
#define INIT_CCCRS	4
#define END_INIT_E	5
#define END_INIT_CTRS	6
#define END_INIT_ESCRS	7
#define END_INIT_CCCRS	8

struct configStruct {
   int eventType;
   int regIndex;
   long mask;
};

struct eventStruct {
      char name[EVT_NAME_LEN];
      int index;
};

struct counterStruct {
   char name[REG_NAME_LEN];
   __u64 addr;
};

struct nonCounterReg {
   char name[REG_NAME_LEN];
   __u64 addr;
   __u64 defaultConfig;
};

union config {
   int flag;
   int count;
   struct eventStruct event;
   struct counterStruct counter;
   struct nonCounterReg escr;
   struct nonCounterReg cccr;
};
*********************************************************/

struct hwctr_dev {
   char events[1024];
   struct cdev hwctr_cdev;
} hdev;

static struct file_operations fops;

static int  
   HwCtr_init( void );

static void 
   HwCtr_finish( void );

static loff_t  
   d_llseek( struct file *filp, loff_t off, int whence );

static ssize_t 
   d_read ( struct file *filp, char *buf, size_t count, loff_t *whence );

static ssize_t 
   d_write ( struct file *filp, const char *buf, size_t count, loff_t *whence);

static int
   d_open  ( struct inode *inode, struct file *filp);

static int
   d_close ( struct inode *inode, struct file *filp);

static int
   d_ioctl (struct inode *i, struct file *f, unsigned int x, unsigned long y);

static unsigned long 
   get_cr4( void );

static int
   setup_cdev (struct hwctr_dev *dev);

static void
   do_wrmsr (unsigned msr, unsigned val);

static void
   setup_escr (unsigned msrAddr, unsigned lowOrderConfig);

/*
  Sets up the CCCR msr for performance counting.
  inputs: cccrAddr - addr of the msr to use for the CCCR
          lowOrderConfig - the bit configuration that is to be written
                     to the low order 32 bits of the CCCR.
*/
static void
   setup_cccr (unsigned cccrAddr, unsigned lowOrderConfig);

/*
  Reads the performance counter specified by the pmcAddr.
  Returns the 40-bit value as an unsigned long
*/
static unsigned long 
   read_counter (unsigned pmcAddr);

/*
  Reads msr specified by msr; stores result in data.
*/
static void
   do_rdmsr (unsigned msr, unsigned long * data);

/* 
  This sets the enable bit in the CCCR.  Once set, counting begins.
*/
static void
   start_counting (void);

/*
  This clears the enable bit in the CCCR.  Once set, counting halts.
*/
static void
   stop_counting (void);

/*
  zero out the ESCR specified by msrAddr, the address of the ESCR
*/
static void
   reset_escr (unsigned msrAddr);

/*
  zero out the CCCR specified by msrAddr, the address of the CCCR
*/
static void
   reset_cccr (unsigned msrAddr);

/*
  zero out the performance counter specified by pmcAddr, the address of
  the counter.
*/
static void
   reset_pmc(unsigned pmcAddr);


static void
   set_cr4_pce (int val);

static void
   set_cr4_tsd (int val);

#define DEVNAME		"HwCtr"
#ifndef HWCTR_NR_DEVS
#define HWCTR_NR_DEVS	1
#endif

#ifndef HWCTR_MAJOR
#define HWCTR_MAJOR	0
#endif

#define UNUSED		0
#define IN_USE		1
/*#define MAX_COUNTERS	32*/
#define MAX_EVENTS	64
#define MAX_POSS_EVENTS	200
#endif
