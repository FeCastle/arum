#if !defined( Events_h )
#define Events_h

#include "Arch.h"
#include "defs.h"

//#define DEBUG_EVENT 

/******************* these are defined in Arch.h ****************************
  struct RegNode {
      int rType;   
      char * name;
      int number;
      unsigned long addr;
      //struct RegNode associates[MAX_REG_TYPES];  //an array of register lists
      //struct RegNode * associates;
      struct RegNode * previous;
      struct RegNode * next;
   };

  struct EventNode {
      char * name;
      int index;
      int setCount;
      struct RegNode registerSets[MAX_REG_SETS];
      struct EventNode * next;
  };
******************************************************************************/

enum regType {PMC, ESCR, CCCR};

class Events {

   public:
 
      Events(const struct ArchId * aid);
      Events();
      ~Events();
      bool isEmpty() const;
      int getCount() const;
      int lookupEventByName(const char * name);
      void printEvents();
      void resetIterator();
      int incrementIterator();
      bool getEventName(char * name);
      int getEventIndex();
      bool getEventCounters(struct counterStruct * eCounters);
      int getEventRegSetCount();
      int getEventRegSetCount(char * name);
      bool getEventRegSets(struct RegNode * eRegSets, struct RegNode * eTail);
      bool getEventRegSets(char * name, struct RegNode * eRegSets, 
                           struct RegNode * eTail);
      bool getEventRegSets(int eIndex, struct RegNode * eRegSets,
                           struct RegNode * eTail);

   private:
      EventNode * head;
      EventNode * tail;
      EventNode * iterator;
      char archStr[MAX_ARCH_FILE_LEN];
      struct ArchId aid;
      int count;
      void createArchStr(char * archStr);
      bool addNode (Arch & a);
      bool removeNode ();
      void walkArchList(const Arch & a);
      
};
#endif //Events

