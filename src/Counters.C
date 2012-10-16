#include "Counters.h"
#include "CpuId.h"
#include "Events.h"
#include "defs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cctype>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

baseCpuid base;                 // to hold basic cpuid info

Counters::Counters ()
{
   memset (&base, '\0', sizeof(struct baseCpuid));
   bzero (this->eventListStr, this->MaxEventsListLen);
   this->eventListStr[this->MaxEventsListLen] = '\0';
   bzero (this->eventReport, this->MaxReportLen);
   this->eventReport[this->MaxReportLen] = '\0';
   this->dbug = false;
   this->mplex = false;
   this->compatible = false;
}

Counters::Counters (const char * evts, bool debug, bool mplex) 
{
   int i, j, len, evtCnt;
   bool archCheck;
   int compat;
   len = strlen (evts);
   if (debug) {
      this->dbug = true;
   } 

   if (mplex) {
      this->mplex = true;
   }

   if (len <= MaxEventsListLen) {
      // 
      //parse events
      //

      //
      //get cpuid info
      // use this to build the ArchId structure to pass to
      // the Events instantiation
      CpuId cpu;                      // runs the cpuid instruction
      cpu.getBasicReport (&base);     // get basic cpuid info

#ifdef DEBUG_COUNT
      if (base.cpuid_enabled) {
         printf ("*************** CPUID Report ***************\n");
         printf ("Vendor: %s\n", base.vendor);
         if (base.brand_string) {
            printf ("Brand String: %s\n", base.brand_string);
         }
         printf ("LSFN        Family      Model       Stepping\n");
         printf ("0x%8.8lx  0x%8.8lx  0x%8.8lx  0x%8.8lx\n\n",
                 base.lsfn, base.family, base.model, base.stepping);
         printf ("Brand Id    Clflush Sz  NB node ID  CPU No.\n");
         printf ("0x%8.8lx  0x%8.8lx  0x%8.8lx  0x%8.8lx\n\n",
                 base.brand_id, base.clflush_size, base.nb_node_id,
                 base.cpu_number);
      } else {
         printf ("no cpuid support!\n");
      }
#endif
      // fill an ArchId structure
      struct ArchId aid; //= {"GenuineIntel", 15, 6};
      aid.family = base.family;
      aid.model = base.model;
      strcpy (aid.vendor, base.vendor);

      //
      //Initialize Events object
      //  Use it to check if event list from user is compatible 
      //  with architecture 
      this->events = Events(&aid);

      ////debug printing
      //this->events.printEvents();

      strcpy(this->eventListStr, evts);
      this->eventListStr[len] = '\0';

      //
      // create a table of the event requests and then check
      // if they are compatible with the architecture
      //
      evtCnt = this->parseEventStr();
      archCheck = true; 
      for (i=0; i < evtCnt; i++) {
         compat = this->events.lookupEventByName(this->reqEventTable[i].name);
         if (compat < 0) {
            archCheck = false;
         } 
         this->reqEventTable[i].index = compat;
      }
      this->compatible = archCheck;

      ////debug printing
      //printf ("Parsed Events Request Table\n");
      //for (i = 0; i < evtCnt; i++) {
      //   printf("   %30.30s\t%d\n", this->reqEventTable[i].name,
      //          this->reqEventTable[i].index);
      //}

      //
      // create a table of possible events
      //
      if (this->compatible) {
         bool ok;

         this->allEventTable =  new eventStruct[events.getCount()];
         this->allEvtTableSz = events.getCount();
         this->events.resetIterator();
         for (i = 0; i < this->allEvtTableSz; i++) { 
            this->allEventTable[i].index = this->events.getEventIndex();
            memset(this->allEventTable[i].name, '\0', MAX_EVT_NAME_LEN);
            ok = this->events.getEventName(this->allEventTable[i].name);
            ////debug printing
            //if (ok) {
            //   printf ("An event: %d  %s\n", this->allEventTable[i].index, 
            //            this->allEventTable[i].name);
            //}
            for (j=0; j<MAX_CTRS_PER_EVT; j++) {
               this->allEventTable[i].ctrs[j] = -1;
            }
            this->events.incrementIterator();
         }
         ////debug printing
         //printf("Possible Events for this Architecture:\n");
         //for (i = 0; i < this->allEvtTableSz; i++) {
         //   printf ("\t%d  %s\n", this->allEventTable[i].index, 
         //           this->allEventTable[i].name);
         //}
      }

   } else {
      error ("constructor: event list exceeds max length.");
   }
}


int 
Counters::parseEventStr()
{
   char * temp;
   int i, j, eventCount, spaces, parsed, len;
   len = strlen(this->eventListStr);
   //printf ("parseEventStr: %s with len %d\n", this->eventListStr, len);
   i = 0;
   spaces = 0;
   while (true) {
      if (this->eventListStr[i] == '\0') {
         break;
      }
      if (isspace(this->eventListStr[i])) { 
         spaces ++;
         i++;
         while (isspace(this->eventListStr[i])) {
            i++;
         }
      } else {
         i++;
      }
   }

   eventCount = spaces + 1;
   //printf ("event count = %d\n", eventCount);
   this->reqEventTable = new eventStruct [eventCount];   
   this->reqEvtTableSz = eventCount;

   temp = new char[MAX_EVT_NAME_LEN];
   memset (temp, '\0', MAX_EVT_NAME_LEN);
   i = 0;
   j = 0;
   parsed = 0;
   while(i <= len){
      if (isspace(this->eventListStr[i])) {
         if (temp != NULL) {
            //this->reqEventTable[parsed].name = new char[strlen(temp) + 1];
            memset (this->reqEventTable[parsed].name, '\0', MAX_EVT_NAME_LEN);
            strcpy(this->reqEventTable[parsed].name, temp);
            this->reqEventTable[parsed].index = -1;
            //printf ("%s\t%d\n",this->reqEventTable[parsed].name, 
            //          this->reqEventTable[parsed].index);
            parsed ++;
         } 
         memset (temp, '\0', MAX_EVT_NAME_LEN);
         i++;
         j = 0;
         while (isspace(this->eventListStr[i])) {
            i++;
         }
      //} else if (this->eventListStr[i] == '\0'){
      } else if (i == len) {
         if (temp != NULL) {
            //this->reqEventTable[parsed].name = new char[strlen(temp) + 1];
            memset (this->reqEventTable[parsed].name, '\0', MAX_EVT_NAME_LEN);
            strcpy(this->reqEventTable[parsed].name, temp);
            this->reqEventTable[parsed].index = -1;
            //printf ("%s\t%d\n",this->reqEventTable[parsed].name, 
            //         this->reqEventTable[parsed].index);
            parsed ++;
            i++;
         }
      }else {
         temp[j] = this->eventListStr[i];
         j++;
         i++;
      }
   } //end while
   //printf ("Counters: parseEventStr: parsed %d events\n", parsed);
   return (parsed);
}


bool 
Counters::isCompatible() const
{
   return this->compatible;
}


Counters::~Counters()
{

}


// Counters::openDevice
// Purpose: open hardware counter device file
// Description: Calls the hardware counter module's open to open the Arum
//              device.  Opens for Read and Write. On success, 1 is returned;
//              on failure, -1 is returned. The Counter class does not force 
//              an exit as a result of this error. 
int
Counters::openDevice()
{
   int rv;
 
   if ((this->fd = open ("/dev/Arum", O_RDWR)) == -1){
      rv = -1;
      this->error("open device failed.");
   } else {
      rv = 1;
   }

//   if ((this->fp = fopen("/dev/Arum", "w+")) == NULL) {
//      rv = -1;
//      this->error("open device failed.");
//   } else {
//      rv = 1;
//   }   
   return rv;
}


// Counters::closeDevice
// Purpose: close hardware counter device file
// Description: Calls the hardware counter module's close to close the Arum
//              device.  If the close fails, an error is reported. The Counter
//              class does not force an exit as result of this error.
int
Counters::closeDevice()
{
   int rv;
   if ((rv = close (this->fd)) == -1) {
      this->error("close device failed.\n");
   }

   //if ((rv = fclose(this->fp)) == EOF) {
   //   this->error("close device failed.\n");
   //}
   return rv;
}

// Counters::writeCounters()
// Purpose: Send the hardware counter module the list of performance events
//          to monitor
// Description: Notifies the hardware counter module of the performance events
//          that are to be monitored (this list is stored in the reqEvtTable). 
//          If the Counters compatible flag is not true, this method returns an
//          an error.  There are two phases to writing.  The first part sends
//          sends the device configuration information for setting up counters 
//          for the architecture.  The second phase tells the device which 
//          registers to configure for the current event set and tells the device
//          to begin counting.  This is also when Counters would tell the device
//          to multiplex over several events (not implemented).
// 
//          On success returns the number of events written.  On error returns 
//          -1. (TODO - return error codes that are more specific).
//
int
Counters::writeCounters()
{
   int rv, i, j, k, eIndex, regsetCount, maxCounter, nonCtrRegCount;
   union counterConfig ucc;
   struct counterStruct * ctrTable;  
   bool reqEvtConf = false;
   bool allReqEvtsConf = true;

   //
   //iterate through events.
   //save counter info in a temp table
   //send counter info to module
   //

   // a temp table
   struct counterStruct * ctrsTable = new struct counterStruct[MAX_COUNTERS];
   if (!ctrsTable) {
      printf ("No memory to allocate ctrsTable\n");
      return -1;
   }
   for (i=0; i<MAX_COUNTERS; i++) {
      ctrsTable[i].index = -1;
   }

   //iterate over events - save counters in inner temp table
   maxCounter = 0;
   this->events.resetIterator();
   for (i = 0; i < this->events.getCount(); i++) {
      regsetCount = this->events.getEventRegSetCount();
      eIndex = this->events.getEventIndex();
      struct counterStruct * ctrs = new struct counterStruct[regsetCount];
      //printf ("Event %d has %d register sets\n",
      //         this->events.getEventIndex(), regsetCount);
      if(!(this->events.getEventCounters(ctrs))) {
        this->error("Counters:writeCounters: failed to get counter data from event list"); 
      }
      for (k=0; k < regsetCount; k++) {
         //printf("%d %s 0x%8.8lx\n", ctrs[k].index, ctrs[k].name, ctrs[k].addr);
         if (ctrs[k].index > maxCounter) {
            maxCounter = ctrs[k].index;
         }
         ctrsTable[ctrs[k].index].index = ctrs[k].index;
         memset(ctrsTable[ctrs[k].index].name, '\0', REG_NAME_LEN);
         strcpy(ctrsTable[ctrs[k].index].name, ctrs[k].name);
         ctrsTable[ctrs[k].index].addr = ctrs[k].addr;
         this->allEventTable[i].ctrs[k] = ctrs[k].index;  //saving counters in allEvtTable
      }
      this->events.incrementIterator();
      delete [] ctrs;
   }

   //// debug printing
   //for (i = 0; i <= maxCounter; i++) {
   //   if (ctrsTable[i].index != -1) {
   //      printf("%d %s 0x%8.8lx\n", 
   //         ctrsTable[i].index, ctrsTable[i].name, ctrsTable[i].addr);
   //   } else {
   //      printf("No entry for counter %d\n", i);
   //   }
   //}

   //send list of coutner data
   memset(&ucc, '\0', sizeof (union counterConfig));
   ucc.control.flag = INIT_CTRS;
   ucc.control.count = maxCounter+1;
   rv = write(this->fd, &ucc, sizeof(union counterConfig));
   if (rv !=1) {
      this->error("Counters:writeCounters:initialization failure");
   } else {
      i = 0;
      //printf ("Counters: writeCounters: initializing counters\n");
      for (i = 0; i <= maxCounter; i++) {
         memset(&ucc, '\0', sizeof(union counterConfig));
         ucc.counter.index = ctrsTable[i].index;
         if (ctrsTable[i].index != -1) {
            ucc.counter.addr = ctrsTable[i].addr;
            memset (ucc.counter.name, '\0', REG_NAME_LEN);
            strcpy(ucc.counter.name, ctrsTable[i].name);
            //printf ("send counter: %d-0x%8.8lx-%s\n", 
            //   ucc.counter.index, ucc.counter.addr, ucc.counter.name);
         } else {
            //printf ("no entry %d\n", ctrsTable[i].index);
            ucc.counter.index = ctrsTable[i].index;
            memset (ucc.counter.name, '\0', REG_NAME_LEN);
         }
         rv = write(this->fd, &ucc, sizeof(union counterConfig));
         if (rv != 1 ){
            printf ("error %d\n", rv);
            perror("writeCounters: initializing counters failed");
            rv = -1;
            break;
         } else {
            //printf ("Counters: writeCounters: count written = %d\n", rv);
         }
      } //end for to initialize counters
   } 
   delete [] ctrsTable;



   //
   //send module table of possible events
   //

   //send the control struct to indicate operation
   memset(&ucc, '\0', sizeof (union counterConfig));
   ucc.control.flag = INIT_EVENTS;
   ucc.control.count = this->allEvtTableSz;
   rv = write(this->fd, &ucc, sizeof(union counterConfig));   
   if (rv != 1) {
      this->error("Counters:writeCounters:initialization failure");
   } else {
      //now send the event structs
      i =0;
      //printf ("Counters: writeCounters: initializing events\n");
      for (i=0; i < this->allEvtTableSz; i++) {
         memset(&ucc, '\0', sizeof (union counterConfig));
         //set event index 
         ucc.event.index = this->allEventTable[i].index;
         memset (ucc.event.name, '\0', EVT_NAME_LEN);
         //set event name
         strcpy(ucc.event.name, this->allEventTable[i].name);
         //set the event counters
         for (k=0; k<MAX_CTRS_PER_EVT; k++) {
            ucc.event.ctrs[k] = this->allEventTable[i].ctrs[k]; 
         }
         //printf ("  send event: %d-%s ", ucc.event.index,
         //   ucc.event.name);
         ////debug printing
         //printf(" with counters ");
         //for (k=0; k<MAX_CTRS_PER_EVT; k++) {
         //   if (ucc.event.ctrs[k] == -1) {
         //      break;
         //   }
         //  printf ("%d ", ucc.event.ctrs[k]);
         //}
         //printf ("\n");

         //send an event
         rv = write(this->fd, &ucc, sizeof(union counterConfig));
         if (rv != 1 ){
            printf ("error %d\n", rv);
            perror("writeCounters: initializing events failed"); 
            rv = -1;
            break;
         } else {
            //printf ("Counters: writeCounters: count written = %d\n", rv);
         }
      } //end for loop to send event setup info
   }


   //
   // prepare the non-counter register data.
   //
   struct RegNode * nonctrRegTable = new struct RegNode[MAX_NONCTR_REGS];
   for (i=0; i < MAX_NONCTR_REGS; i++) {
      nonctrRegTable[i].rType = -1;
   }
   this->events.resetIterator();
   j = 0;
   for (i = 0; i < this->events.getCount(); i++) {
      regsetCount = this->events.getEventRegSetCount();
      struct RegNode * regsets = new struct RegNode[regsetCount];
      struct RegNode * eTail;
      this->events.getEventRegSets(regsets, eTail);
      ////debug printing
      //char * name = new char[EVT_NAME_LEN];
      //memset (name, '\0', EVT_NAME_LEN);
      //this->events.getEventName(name);
      //printf("Event:%s\n",name);
      //for (k=0; k < regsetCount; k++) {
      //   printf("type:%d name:%s number:%d addr:0x%8.8lx\n", 
      //          regsets[k].rType, regsets[k].name, regsets[k].number,
      //          regsets[k].addr);
      //   eTail = regsets[k].next;
      //   while (eTail != NULL) {
      //      printf(" type:%d name:%s number:%d addr:0x%8.8lx\n",
      //          eTail->rType, eTail->name, eTail->number,
      //          eTail->addr);
      //      eTail = eTail->next;
      //   }
      //}
      //delete [] name;

                                //for this event's register sets, save the 
                                //non-counter registers in the non-counter 
                                //reg table IF the register has not already
                                //been saved. j is position in nonctrRegTable.
      for (k=0; k < regsetCount; k++) {
         bool found = false;
         if (regsets[k].rType != PMC) {
            for (int l = 0; l < j; l++) {
               if (nonctrRegTable[l].addr == regsets[k].addr) {
                  found = true;
                  break;
               }
            } 
            if (!found) {
               nonctrRegTable[j].rType = regsets[k].rType;
               nonctrRegTable[j].number = regsets[k].number;
               nonctrRegTable[j].addr = regsets[k].addr;
               nonctrRegTable[j].name =  new char[REG_NAME_LEN];
               strcpy(nonctrRegTable[j].name, regsets[k].name);
               nonctrRegTable[j].next = NULL;    //ignoring next field
               j++;
            } else {
               found = false;
            }
         }
         eTail = regsets[k].next;
         while (eTail != NULL) {
            if (eTail->rType != PMC){
               for (int l = 0; l < j; l++) {
                  if (nonctrRegTable[l].addr == eTail->addr) {
                     found = true;
                     break;
                  }
               }
               if (!found) {
                  nonctrRegTable[j].rType = eTail->rType;
                  nonctrRegTable[j].number = eTail->number;
                  nonctrRegTable[j].addr = eTail->addr;
                  nonctrRegTable[j].name =  new char[REG_NAME_LEN];
                  strcpy(nonctrRegTable[j].name, eTail->name);
                  nonctrRegTable[j].next = NULL;    //ignoring next field
                  j++;
               } else {
                  found = false;
               }
            }
            eTail = eTail->next;
         }
      }            
      this->events.incrementIterator(); 
      delete [] regsets;  //(incorrect - not deleting internal nodes)
   }
   ////debug printing
   //for (i=0; i<j; i++) {
   //   printf("type:%d name:%s number:%d addr:0x%8.8lx\n",
   //           nonctrRegTable[i].rType,  nonctrRegTable[i].name, 
   //          nonctrRegTable[i].number, nonctrRegTable[i].addr);
   //}

   //
   // send each type of non-counter register info.
   //
   for (i=1; i<MAX_REG_TYPES; i++) {
      nonCtrRegCount = 0;
      for( int m=0; m<j; m++) {               //determine how many of type 'i'
         if (nonctrRegTable[m].rType == i) {
            nonCtrRegCount ++;
         }
      }                                        //setup the control struct
      memset(&ucc, '\0', sizeof (union counterConfig));
      if (i == ESCR) {
         ucc.control.flag = INIT_ESCRS;
      } else if (i == CCCR) {
         ucc.control.flag = INIT_CCCRS; 
      } else {
         this->error("Counters:writeCounters: 1-unsupported regtype\n");
         return -1;
      }
      ucc.control.count = nonCtrRegCount;
                                               //send control struct
      rv = write(this->fd, &ucc, sizeof(union counterConfig));
      if (rv != 1) {
         this->error("Counters:writeCounters:1-initialization failure");
         return -1;
      } else {                                //send the non-counter reg data
                                              //of type 'i' that is saved in the
                                              //nonctrRegTable
         for (int m=0; m<j; m++) { 
            memset(&ucc, '\0', sizeof(union counterConfig));
            if (i==ESCR) {
               if (nonctrRegTable[m].rType == ESCR) {
                  ucc.escr.addr = nonctrRegTable[m].addr;
                  ucc.escr.defaultConfig = 0;
                  memset (ucc.escr.name, '\0', REG_NAME_LEN);
                  strcpy(ucc.escr.name, nonctrRegTable[m].name);
                  rv = write(this->fd, &ucc, sizeof(union counterConfig));
                  if (rv != 1 ){
                     printf ("error %d\n", rv);
                     perror("writeCounters:2-initializing counters failed");
                     return -1;
                  } else {
                     //printf ("Counters: writeCounters: count written = %d\n", rv);
                  }                  
               } else {
                  continue;
               }
            } else if (i == CCCR) {
               if (nonctrRegTable[m].rType == CCCR) {
                  ucc.cccr.addr = nonctrRegTable[m].addr;
                  ucc.cccr.defaultConfig = 0;
                  memset (ucc.cccr.name, '\0', REG_NAME_LEN);
                  strcpy(ucc.cccr.name, nonctrRegTable[m].name);
                  rv = write(this->fd, &ucc, sizeof(union counterConfig));
                  if (rv != 1 ){
                     printf ("error %d\n", rv);
                     perror("writeCounters:3-initializing counters failed");
                     return -1;
                  } else {
                     //printf ("Counters: writeCounters: count written = %d\n", rv);
                  }
               } else {
                  continue;
               }
            } else {
               this->error("Counters:writeCounters: 2-unsupported regtype\n");
               return -1;
            }
         }
      } 
   } // end for to send non-counter reg data
   delete [] nonctrRegTable;



   //
   // now send the events to count configuration data
   //
   memset(&ucc, '\0', sizeof (union counterConfig));
   ucc.control.flag = CONFIG_EVENTS;
   ucc.control.count = 1; //this->reqEvtTableSz;
   rv = write(this->fd, &ucc, sizeof(union counterConfig));
   if (rv != 1) {
      this->error("Counters:writeCounters:initialization failure");
   } else {
      //get the register sets for first requested event:
      //use getEventRegSets that takes a name or an index;
      regsetCount = this->events.getEventRegSetCount(this->reqEventTable[0].name);
      struct RegNode * regsets = new struct RegNode[regsetCount];
      struct RegNode * eTail;
      this->events.getEventRegSets(this->reqEventTable[0].name, regsets, eTail);

      ////debug printing
      //printf("Event:%s\n",this->reqEventTable[0].name);
      //for (k=0; k < regsetCount; k++) {
      //   printf("type:%d name:%s number:%d addr:0x%8.8lx\n",
      //          regsets[k].rType, regsets[k].name, regsets[k].number,
      //          regsets[k].addr);
      //   eTail = regsets[k].next;
      //   while (eTail != NULL) {
      //      printf(" type:%d name:%s number:%d addr:0x%8.8lx\n",
      //          eTail->rType, eTail->name, eTail->number,
      //          eTail->addr);
      //      eTail = eTail->next;
      //   }
      //}

      //try the event's first register set:
      for (k=0; k < regsetCount; k++) {
         memset(&ucc, '\0', sizeof (union counterConfig));
         ucc.config.eIndex = this->reqEventTable[0].index;
         ucc.config.escrFlag = ucc.config.cccrFlag = 0;
         if (regsets[k].rType == PMC) {
            ucc.config.cNumber = regsets[k].number;
         } else if (regsets[k].rType == ESCR) {
            ucc.config.escrFlag = 1;
            ucc.config.escrAddr = regsets[k].addr;
            ucc.config.escrConfig = 0;
         } else if (regsets[k].rType == CCCR) {
            ucc.config.cccrFlag = 1;
            ucc.config.cccrAddr = regsets[k].addr;
            ucc.config.cccrConfig = 0;
         } else {
            this->error("Counters:writeCounters: unsupported regtype");
            return -1;
         }
         eTail = regsets[k].next;
         while (eTail != NULL) {
            if (eTail->rType == PMC) {
               ucc.config.cNumber = eTail->number;
            } else if (eTail->rType == ESCR) {
               ucc.config.escrFlag = 1;
               ucc.config.escrAddr = eTail->addr;
               ucc.config.escrConfig = 0;
            } else if (eTail->rType == CCCR) {
               ucc.config.cccrFlag = 1;
               ucc.config.cccrAddr = eTail->addr;
               ucc.config.cccrConfig = 0;
            } else {
               this->error("Counters:writeCounters: unsupported regtype");
               return -1;
            }
            eTail = eTail->next;
         }// end while
         ////debug printing
         //printf("Config Structure: eIndex:%d cNum:%d escrFlag:%d cccrFlag:%d\n",
         //       ucc.config.eIndex, ucc.config.cNumber, ucc.config.escrFlag,
         //       ucc.config.cccrFlag);
         //printf("   escrAddr:0x%8.8lx escrConfig:0x%16.16lx\n",
         //       ucc.config.escrAddr, ucc.config.escrConfig);
         //printf("   cccrAddr:0x%8.8lx cccrConfig:0x%16.16lx\n",
	 //       ucc.config.cccrAddr, ucc.config.cccrConfig);
         
         //send this config structure - if success, break from for loop; if fails, continue
         rv = write(this->fd, &ucc, sizeof(union counterConfig));
	if (rv != 1) {
            reqEvtConf = false;
            continue; 
         } else {
            reqEvtConf = true;
            break;
         }
      }//end for
      if (reqEvtConf == false) {
         allReqEvtsConf = false;
      }
      if (allReqEvtsConf != true) {
         this->error("Counters:writeCounters: one or more of the requested events not configured");
      }
   }
   return rv;
}


// Counters::readCounters()
// Purpose: Receive results from the monitored events. 
// Description: Uses the hardware module's read() to get the results from
//          monitoring the selected events. The results are stored in this
//          object and can subsequently by printed via printReport
//          On success, returns the number of events read.  On, error returns 
//          -1.
//
int
Counters::readCounters()
{
   int rv;
   testStruct B; 

   //rv = read(this->fd, this->eventReport, this->MaxReportLen);
   rv = read(this->fd, &B, this->MaxReportLen);
   if (rv < 0) {
      this->error("readCounters failed.");
   } else if (rv == 0) {
      this->debug("readCounters reached end of file.");
   } else {
      printf ("Counters: readCounters: count read = %d\n\tdata read: %d %d %ld\n", 
               rv,B.eventType, B.regIndex, B.mask);
   }

   //rv = fread(&B, sizeof(testStruct), 1, this->fp);
   //if (rv != 1) {
   //   this->error("readCounters failed.");
   //} else {
   //   printf ("Counters: readCounters: count read = %d\n", rv);
   //   printf ("Event Type:%d  RegIndex:%d  RegMask:%ld\n", 
   //            B.eventType, B.regIndex, B.mask);
   //}
   

   return rv;
}

// Counters::printReport()
// Purpose: Display hardware performance counter results
// Description: Incomplete
//
void
Counters::printReport()
{
   printf ("Performance Counter Monitoring Results\n");
   printf ("    %s\n", this->eventReport);
}

// Counters::printStats()
// Purpose: helper method to print out what is known about what hardware events 
//          will be monitored
// Description:
//
void
Counters::printStats()
{
   printf ("Events: %s\n", eventListStr);
}

// Counters::error()
// Purpose: Report an error description when Counters detects an error.
//
void
Counters::error(const char * errorStr)
{
   printf ("Counters: %s\n", errorStr);
}

// Counters::debug()
// Purpose: Print a debug statement if Counters instance has been
//          initialized with debug set to true.
void 
Counters::debug(const char * debugStr)
{
   if (this->dbug) {
      printf("DEBUG: Counters: %s\n");
   }
}
