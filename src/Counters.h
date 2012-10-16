#if !defined( Counters_h )
#define Counters_h

#include <stdio.h>
#include "Arch.h"
#include "defs.h"
#include "Events.h"
#define MAX_NONCTR_REGS	1000
//#define DEBUG_COUNT

   //struct event {
   //   char name[MAX_EVT_NAME_LEN];
   //   int index;
   //};

   struct testStruct {
      int eventType;
      int regIndex;
      long mask;
   };

class Counters {
   //struct event {
   //   char name[MAX_EVT_NAME_LEN];
   //   int index;
   //};

public:
    static const long MaxEventsListLen   = 1024; 
    static const long MaxReportLen       = 8191;

    Counters (const char * evts, bool debug, bool mplex);
    Counters();
    ~Counters();
    int openDevice();
    int closeDevice ();
    int writeCounters();
    int readCounters();
    void printReport();
    void printStats();
    bool isCompatible() const;
private:
   char eventListStr[MaxEventsListLen + 1];  
   FILE * fp;
   int fd;
   bool dbug;
   bool mplex;
   bool compatible;
   Events events;
   eventStruct * reqEventTable;
   eventStruct * allEventTable;
   int reqEvtTableSz;
   int allEvtTableSz;
   char eventReport[MaxReportLen + 1];
   void error (const char * errorStr);
   void debug (const char * debugStr);
   int parseEventStr();
};

#endif
