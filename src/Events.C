//
// Events.C
//
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cctype>
#include <string.h>
#include "Events.h"
#include "Arch.h"

using namespace std; 

/***************************** main test *************************
int main(int argc, char * argv[])
{

   struct ArchId aid = {"GenuineIntel", 15, 6};

   Events events(&aid);
   events.printEvents();

   //events.lookupEventByName("instructions_completed");
   //events.lookupEventByName("instructions_on_vacation");
   //events.lookupEventByName("global_power_events");
   return 0;
}
******************************************************************/
Events::Events()
{
   this->head = this->tail = this->iterator = NULL;
   this->count = 0;
}

Events::Events(const struct ArchId * thatAid)
{
   int rv;

   memset(this->archStr, '\0', MAX_ARCH_FILE_LEN);
   this->head = this->tail = this->iterator = NULL;
   this->count = 0;
   strcpy(this->aid.vendor, thatAid->vendor);
   this->aid.model = thatAid->model;
   this->aid.family = thatAid->family;
   this->createArchStr(this->archStr); 
 
   Arch archie(this->archStr); 
   //read file
   if (archie.readFile()){
       //printf ("file read!\n");
       //this->walkArchList(archie);
   } else {

   }

#ifdef DEBUG_EVENT
   printf ("Events Instantiation of vendor %s family %d model %d!\n",
           thatAid->vendor, thatAid->family, thatAid->model);
   printf ("Architecture String: %s\n", this->archStr);
   printf ("There are %d archie nodes\n", archie.getCount());
#endif

   archie.resetIterator();
   rv = true;
   while (rv == true) {   //event successfully added
      rv = this->addNode(archie);
   }
   
}

void
Events::walkArchList(const Arch & a)
{
  a.printNodes();
}

//
// returns true if parseEvent successfully completed an event and if the
// new node could be created; otherwise returns false
//
bool
Events::addNode(Arch & a)
{
    bool rv;

    EventNode * add = new EventNode;
    if (add == NULL) { // no memory
        rv = false;
    } else {
        add->setCount = 0;
        rv = a.parseEvent(add);
        if (rv != true) {
           //printf ("EVENTS:addNode: call to parseEvent returned false!!\n");
           delete add;
        }
        if (rv == true) {
           add->next = NULL;
           add->index = this->count;
           this->count++;
           if (this->head == NULL) {
               this->head = add;
           } else {
               this->tail->next = add;
           }
           this->tail = add;
        }
    }
    return rv;
}

//
// creates an architecture string based on contents of
// vendor, family, and model. the arch string is used
// as the architecture specific file to read
//
void
Events::createArchStr(char * archStr)
{
   int n;
   memset (archStr, '\0', MAX_ARCH_FILE_LEN);
   if (strcmp(this->aid.vendor,  INTEL_VENDOR_STR ) == 0) {
      n = sprintf (archStr, "Intel-%d-%d", this->aid.family, 
                   this->aid.model);
   } else if (strcmp(this->aid.vendor, AMD_VENDOR_STR) == 0) {
      n = sprintf (archStr, "AMD-%d-%d", this->aid.family, 
                   this->aid.model);
   } else {

   }
}

//
// removes the head node from the list of EventNode nodes
//
bool
Events::removeNode()
{
    bool rv;

    if (this->head == NULL) {
        rv = false;
    } else {
        EventNode * temp = this->head;
        this->head = this->head->next;
        delete temp;
        this->count--;
        if (this->count == 0) {
            this->tail = NULL;
        }
        rv = true;
    }
    return rv;
}

Events::~Events()
{
}


bool
Events::isEmpty() const
{
   bool rv;

   if (this->count > 0) {
      rv = false;
   } else {
      rv = true;
   }
   return rv;
}


//
// returns number of EventNode nodes in the list
//
int
Events::getCount() const
{
   return this->count;
}


//
// return positive index of event if the event with name specified exists
// in EventNode list; otherwise return negative
//
int 
Events::lookupEventByName(const char * name)
{
   int index = -1;
   EventNode * walker = this->head;
   while (walker != NULL) {
      if (strcmp(name,walker->name) == 0) {
         index = walker->index;
         break;
      }
      walker = walker->next;
   }
   //printf ("Event %s found status: %d\n", name, (int)found);
   return index;
}


//
// Walk EventNode list, printing each event and its register sets
//
void 
Events::printEvents()
{
    int i;
    printf ("Events:\n");
    if (!this->isEmpty()) {
        EventNode * iterator = this->head;
        while (iterator != NULL) {
            printf ("%s-%d has %d register sets:\n", iterator->name, 
                    iterator->index, iterator->setCount);
            for (i= 0; i < iterator->setCount; i++){
               printf("Set%d:\ttype:%d name:%s number:%d addr:0x%8.8lx\n", 
                  i, iterator->registerSets[i].rType, 
                  iterator->registerSets[i].name, 
                  iterator->registerSets[i].number,
                  iterator->registerSets[i].addr);
               RegNode * regIt = iterator->registerSets[i].next;
               while (regIt != NULL) {
                  printf("\ttype:%d name:%s number:%d addr:0x%8.8lx\n", 
                     regIt->rType, regIt->name, regIt->number, regIt->addr);
                  regIt = regIt->next;
               }
               printf("\n");
            }
            iterator = iterator->next;
        }       
    } else {
        printf ("Event List is empty\n");
    }
    
}

void 
Events::resetIterator()
{
   this->iterator = this->head;
}


int 
Events::incrementIterator()
{
   int rv;
   if (!this->isEmpty()) {
      if (this->iterator == NULL) {
         rv = E_NULL_NODE;
      }else if (this->iterator->next == NULL) {
         rv = E_END_OF_LIST;
      } else {
         this->iterator = this->iterator->next;
         rv = OK;
      }
   } else {
      printf ("Event List is empty\n");
      rv = E_EMPTY_LIST;
   }
   return rv;
}

//
// gets the name of the current iterator
// Returns false if the list is empty or the current iterator is null.
// Returns true on success and copies the current iterator name into name.
bool 
Events::getEventName(char * name)
{
   bool rv = true;
   if (!this->isEmpty()){
      if (this->iterator == NULL) {
         printf ("Events:getEventName: Try resetting Event iterator\n");
         rv = false;
      } else {
         strcpy(name, this->iterator->name);
      }
   } else {
      printf ("Event List is empty\n");
      rv = false;
   }
   return rv;
}

//
// get the index of the current iterator
// Returns the index of the current iterator.  If the list is empty or
// the current iterator is null, returns a negative number to indicate failure.
int 
Events::getEventIndex()
{
   int rv;
   if (!this->isEmpty()){
      if (this->iterator == NULL) {
         printf ("Events:getEventIndex: Try resetting Event iterator\n");
         rv = E_NULL_NODE;
      } else {
         rv = this->iterator->index;
      }
   } else {
      printf ("Events:getEventIndex: List is empty\n");
      rv = E_EMPTY_LIST;
   }
   return rv;
}

//
// Return the counter information for the current iterator into
// the array of counterStructs.  Return true on success and 
// false otherwise.
//
bool 
Events::getEventCounters(struct counterStruct * eCounters)
{
   bool rv = true;
   int i;
   int j = 0;
   if (!this->isEmpty()) {
      if (this->iterator == NULL) {
          printf ("Events:getEventCounters: Try resetting Event iterator\n");
          rv = false;
      } else {
         //create a regnode iterator to walk down the register set lists
         for (i = 0; i < this->iterator->setCount; i++) {
            if (iterator->registerSets[i].rType == PMC) {
               eCounters[j].index = this->iterator->registerSets[i].number;
               memset(eCounters[j].name, '\0', REG_NAME_LEN);
               strcpy(eCounters[j].name, this->iterator->registerSets[i].name);
               eCounters[j].addr = this->iterator->registerSets[i].addr;
               j++;
            } else {
               RegNode * regIt = this->iterator->registerSets[i].next;
               while (regIt != NULL) {
                  if (regIt->rType == PMC) {
                     eCounters[j].index = regIt->number;
                     memset(eCounters[j].name, '\0', REG_NAME_LEN);
                     strcpy(eCounters[j].name, regIt->name);
                     eCounters[j].addr = regIt->addr;
                     j++;
                     break;
                  } else {
                     regIt = regIt->next;
                  }
               }
            } 

         } //end for
      }
   } else {
      printf ("Events:getEventCounters: List is empty\n");
      rv = false;
   }
   return rv;
}

int 
Events::getEventRegSetCount() 
{
   int rv;

   if (!this->isEmpty()) {
      if (this->iterator == NULL) {
         printf ("Events:getEventRegSetCount: Try resetting Event iterator\n");
         rv = E_NULL_NODE;
      } else {
         rv = this->iterator->setCount;
      }
   } else {
      printf ("Events:getEventRegSetCount: List is empty\n");
      rv = E_EMPTY_LIST;
   }
   return rv;
}


int
Events::getEventRegSetCount(char * name)
{
   int rv;
   bool found = false;
   if (!this->isEmpty()) {
      this->resetIterator();
      while (this->iterator != NULL) {
         if (strcmp(name, this->iterator->name) == 0) {
            found = true;
            break;
         }
         this->incrementIterator();
      }
      if (found == false) {
         printf("Events:getEventRegSetCount-byName: name not found\n");
         rv = E_NO_NAME;
      } else {
         rv = this->iterator->setCount;
      }
   } else {
      printf ("Events:getEventRegSetCount-byName: List is empty\n");
      rv = E_EMPTY_LIST;
   }
   return rv;
}



//
// Return the register sets for the current Event iterator into
// the array of RegNode structures.  Return true on success and
// false otherwise.
//
bool 
Events::getEventRegSets(struct RegNode * eRegSets, struct RegNode * eTail)
{
   bool rv = true;
   int i, rsCount;
   if (!this->isEmpty()) {
      if (this->iterator == NULL) {
         printf ("Events:getEventRegSets: Try resetting Event iterator\n");
         rv = false; 
      } else {
         rsCount = this->iterator->setCount;
         for (i = 0; i < rsCount; i++) {
            //printf ("Starting iteration at eRegSets[%d]\n", i);
            eRegSets[i].rType = this->iterator->registerSets[i].rType;
            eRegSets[i].number = this->iterator->registerSets[i].number;
            eRegSets[i].addr = this->iterator->registerSets[i].addr;
            eRegSets[i].name = new char[EVT_NAME_LEN];
            strcpy(eRegSets[i].name, this->iterator->registerSets[i].name); 
            RegNode * iWalker; 
            if(this->iterator->registerSets[i].next != NULL) {
               iWalker = this->iterator->registerSets[i].next;   
            } else {
               continue;
            }
            eRegSets[i].next = new RegNode;
            eRegSets[i].next->rType = iWalker->rType;
            eRegSets[i].next->number = iWalker->number;
            eRegSets[i].next->addr = iWalker->addr;
            eRegSets[i].next->name = new char[EVT_NAME_LEN];
            strcpy(eRegSets[i].next->name, iWalker->name);
            eRegSets[i].next->next = NULL;
            eTail = eRegSets[i].next;
            iWalker = iWalker->next;
            while (iWalker != NULL) {
               RegNode * add = new RegNode;
               add->rType = iWalker->rType;
               add->number = iWalker->number;
               add->addr = iWalker->addr;
               add->name = new char[REG_NAME_LEN];
               strcpy(add->name, iWalker->name);
               add->next = NULL;
               if (eTail == NULL) {
                  //printf("etail is NULL - pointing it to add\n");
                  eTail = add;
               } else {
                  //printf("eTail not NULL - pointing its next to add\n");
                  eTail->next = add;
                  eTail = add;
               }
               iWalker = iWalker->next;
            }          
         } 
      }
      ////debug printing
      //printf("Events:getEventRegSets:\n");
      //for (i = 0; i < rsCount; i++) {
      //   printf("type:%d name:%s number:%d addr:0x%8.8lx\n",
      //          eRegSets[i].rType, eRegSets[i].name, eRegSets[i].number,
      //          eRegSets[i].addr); 
      //   eTail = eRegSets[i].next;
      //   while (eTail != NULL) {
      //      printf("type:%d name:%s number:%d addr:0x%8.8lx\n",
      //          eTail->rType, eTail->name, eTail->number,
      //          eTail->addr);
      //      eTail = eTail->next;
      //   }
      //   if (eTail == NULL) {
      //      printf("eTail is NULL!\n");
      //   }
      //}

   } else {
      printf ("Events:getEventRegSets: List is empty\n");
      rv = false;
   }
   return rv;
}


//
// Return the register sets for the Event with name 'name'. Return
// data into the array of RegNode structures.  Return true on success 
// and false otherwise.
//
bool 
Events::getEventRegSets(char * name, struct RegNode * eRegSets,
                           struct RegNode * eTail)
{
   bool rv = true;
   bool found = false;
   int i, rsCount;
   if (!this->isEmpty()) {
      this->resetIterator();
      while(this->iterator !=NULL) {
         if (strcmp(name, this->iterator->name) == 0) {
            found = true;
            break;
         } 
         this->incrementIterator();
      }
      if (found != true) {
         printf("Events:getEventRegSets-byName: name not found\n");
         rv = false;
      } else {
         rsCount = this->iterator->setCount;
         for (i = 0; i < rsCount; i++) {
            //printf ("Starting iteration at eRegSets[%d]\n", i);
            eRegSets[i].rType = this->iterator->registerSets[i].rType;
            eRegSets[i].number = this->iterator->registerSets[i].number;
            eRegSets[i].addr = this->iterator->registerSets[i].addr;
            eRegSets[i].name = new char[EVT_NAME_LEN];
            strcpy(eRegSets[i].name, this->iterator->registerSets[i].name);
            RegNode * iWalker;
            if(this->iterator->registerSets[i].next != NULL) {
               iWalker = this->iterator->registerSets[i].next;
            } else {
               continue;
            }
            eRegSets[i].next = new RegNode;
            eRegSets[i].next->rType = iWalker->rType;
            eRegSets[i].next->number = iWalker->number;
            eRegSets[i].next->addr = iWalker->addr;
            eRegSets[i].next->name = new char[EVT_NAME_LEN];
            strcpy(eRegSets[i].next->name, iWalker->name);
            eRegSets[i].next->next = NULL;
            eTail = eRegSets[i].next;
            iWalker = iWalker->next;
            while (iWalker != NULL) {
               RegNode * add = new RegNode;
               add->rType = iWalker->rType;
               add->number = iWalker->number;
               add->addr = iWalker->addr;
               add->name = new char[REG_NAME_LEN];
               strcpy(add->name, iWalker->name);
               add->next = NULL;
               if (eTail == NULL) {
                  //printf("etail is NULL - pointing it to add\n");
                  eTail = add;
               } else {
                  //printf("eTail not NULL - pointing its next to add\n");
                  eTail->next = add;
                  eTail = add;
               }
               iWalker = iWalker->next;
            } //end while
         } //end for
      } //end found else
   } else {
      printf ("Events:getEventRegSets: List is empty\n");
      rv = false;
   }
   return rv;
}


//
// Return the register sets for the Event with index 'name'. Return
// data into the array of RegNode structures.  Return true on success
// and false otherwise.
//
bool 
Events::getEventRegSets(int eIndex, struct RegNode * eRegSets,
                           struct RegNode * eTail)
{
   return true;
}
