#if !defined(defs_h)
#define defs_h
#include <linux/fs.h>
#include <asm/types.h>

#define EVT_NAME_LEN    30
#define REG_NAME_LEN    20
#define MAX_COUNTERS	32
#define MAX_CTRS_PER_EVT	10
#define MAX_REG_TYPES	3

#define NO_STATE 	0
#define INIT_EVENTS     1
#define INIT_CTRS       2
#define INIT_ESCRS      3
#define INIT_CCCRS      4

#define END_INIT_EVENTS 6	
#define END_INIT_CTRS   7 
#define END_INIT_ESCRS  8 
#define END_INIT_CCCRS  9 

#define CONFIG_EVENTS	10
#define START_COUNTERS	15
#define COUNTING	20	

//#define INIT		1

struct configStruct {
   int eventType;
   int regIndex;
   long mask;
};

struct controlStruct {
   int flag;
   int count;
};

struct eventStruct {
      char name[EVT_NAME_LEN];
      int ctrs[MAX_CTRS_PER_EVT];
      int index;
};

struct counterStruct {
   int index;
   char name[REG_NAME_LEN];
   long unsigned int addr;
};

struct nonCounterReg {
   char name[REG_NAME_LEN];
   long unsigned int addr;
   long unsigned int defaultConfig;
   int inUse;
};

struct configEvent {
   int eIndex;
   int cNumber;
   int escrFlag;
   int cccrFlag;
   long unsigned int escrAddr;
   long unsigned int escrConfig;
   long unsigned int cccrAddr;
   long unsigned int cccrConfig;
};

union counterConfig {
   struct controlStruct control;
   struct eventStruct event;
   struct counterStruct counter;
   struct nonCounterReg escr;
   struct nonCounterReg cccr;
   struct configEvent config;
};

struct possEvent {
   char name[EVT_NAME_LEN];
   int ctrs[MAX_CTRS_PER_EVT];
};

struct reg {
   long unsigned int addr;
   char name[REG_NAME_LEN];
};
#endif /*defs_h*/
