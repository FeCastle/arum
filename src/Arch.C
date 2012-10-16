//
// Arch.C
//

#include "Arch.h"
#include "defs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cctype>
#include <iostream>
#include <fstream>
using namespace std;


Arch::Arch(const char * archStr)
{
   this->head = this->tail = this->iterator = NULL;
   this->count = 0;
   this->archId = determineArchitecture(archStr);
   //printf ("str: %s id: %d\n", archStr, this->archId);
}


Arch::~Arch()
{
}

bool
Arch::readFile()
{
   bool rv;
   char filename[MAX_ARCH_FILE_LEN];
   char line [MAX_LINE_LEN];
   ifstream fin;

   memset (filename, '\0', MAX_ARCH_FILE_LEN);
  
   if (this->archId == INTEL_15_6_ID) {
      strcpy(filename, INTEL_15_6_STR);
   } else if (this->archId == AMD_15_65_ID) {
      strcpy (filename, AMD_15_65_STR);
   } else {
      return false;
   }

   fin.open (filename, ios_base::in);
   if (!fin.is_open()){
      printf("Arch::readFile: open failed %s\n", filename);
      rv = false;
   } else {
      while (fin.getline(line, MAX_LINE_LEN, '\n')){
         if ((line[0] == '#') || (iscntrl(line[0]))) {
            continue;                    //ignore comments and blank lines
         }else {
            this->addLine(line);
            //printf ("%s\n", line);
         }
      }
      fin.close();
      rv = true;
   }
   return rv;
}

//
// returns true if an event is successfully parsed;
// otherwise returns false
//
bool 
Arch::parseEvent(EventNode * e)
{
   bool rv = true;
   bool firstInSet = false;
   int i,j, iType, iNum;
   unsigned long lAddr;
   char cType[MAX_RTYPE_AS_CHAR_LEN];
   char cNum[MAX_CTRNUM_AS_CHAR_LEN];
   char cAddr[MAX_ADDR_AS_CHAR_LEN];
   char tempName[MAX_LINE_LEN];   

   //make sure iterator points to top of an event
   if (this->iterator->line[0] != '!') {
#ifdef DEBUG_ARCH
      printf ("1. Format of \"%s\" is incorrect in file %s\n", 
         this->iterator->line, this->archStr);
#endif
      rv = false;  //could not add an event

   //fill in the event
   } else {
      i=0;
      //get the event name
      e->name = new char[30];
      memset(e->name, '\0', 30);
      i++;;
      while(this->iterator->line[i] != '\0') {
         e->name[i-1] = this->iterator->line[i];
         i++;
      }
      e->setCount = 0;
      if (this->incrementIterator() != OK) {
#ifdef DEBUG_ARCH
          printf("1. ARCH:parseEvent call to incrementIterator failed\n");
#endif
          return false;  //could not add an event
      }

      //
      //parse register lines
      //
      while (true) {
         if (this->iterator->line[0] != 'r') {  //not a register line
#ifdef DEBUG_ARCH
            printf("2. ARCH:parseEvent curr line is not a register line\n");
            printf("   curr line starts with %c\n",this->iterator->line[0]);
#endif
            return true;  //an event has ended
         }
         firstInSet = true; 
         i = 0;
         j = 0;

         //parse the pieces of a register line
         while (true) {
            //get the type
            j=0;
            if ( (this->iterator->line[i] != 'r') &&
                 (!isdigit(this->iterator->line[i+1])) ) {
#ifdef DEBUG_ARCH
               printf (
                  "2. Format for entry %s in Events file %s is not supported\n",
                  this->iterator->line, this->archStr);
#endif
               return false; //format error - event should not be added
            } else {
               i++;
               cType[j] = this->iterator->line[i];
               cType[j+1] = '\0';
               iType = atoi(cType);
               if ( (iType < 0) || (iType >= MAX_REG_TYPES) ) {
#ifdef DEBUG_ARCH
                  printf (
                  "3. Format for entry %s in Events file %s is not supported\n",
                     this->iterator->line, this->archStr);
#endif
                     return false;   //format error
               }
               i++;
            }
            if (this->iterator->line[i] != ':') {
#ifdef DEBUG_ARCH
               printf (
                  "4. Format for entry %s in Events file %s is not supported\n",
                  this->iterator->line, this->archStr);
#endif
               return false;   //format error
            }
            i++;

            //get the name
            j=0;
            memset(tempName, '\0', MAX_LINE_LEN);
            while((this->iterator->line[i] != '/') && (i < MAX_LINE_LEN)) {
              tempName[j] = this->iterator->line[i]; 
              //if (this->iterator->line[i] = '\0') {
              //   break;
              //}
              i++;
              j++;
            }
            tempName[j] = '\0';
            //printf("%s  ", tempName);

            //get the number
            j=0;
            if (this->iterator->line[i] != '/') {
#ifdef DEBUG_ARCH
                printf (
                  "5. Format for entry %s in Events file %s is not supported\n",
                   this->iterator->line, this->archStr);
#endif
               return false; //format error
            }
            i++;            
            while (this->iterator->line[i] != '/'){
               if (!isdigit(this->iterator->line[i]) ||
                  (j == MAX_CTRNUM_AS_CHAR_LEN)) {
#ifdef DEBUG_ARCH
                   printf (
                  "6. Format for entry %s in Events file %s is not supported\n",
                   this->iterator->line, this->archStr);
#endif
                   return false;  //format error
               }
               cNum[j] = this->iterator->line[i];
               i++;
               j++;
            } 
            cNum[j] = '\0';
            iNum = atoi(cNum);
            if ( iNum < 0 ) {
#ifdef DEBUG_ARCH
               printf (
                  "7. Format for entry %s in Events file %s is not supported\n",
                   this->iterator->line, this->archStr);
#endif
                   return false;  //format error
            }


            //get the addr
            j=0;
            if(this->iterator->line[i] != '/') {
#ifdef DEBUG_ARCH
               printf (
                 "8. Format for entry %s in Events file %s is not supported\n",
                   this->iterator->line, this->archStr);
#endif
               return false;  //format error
            } 
            i++;
            while ((this->iterator->line[i] != 'h') && 
                   (i != MAX_ADDR_AS_CHAR_LEN)) {
               cAddr[j] = this->iterator->line[i];
               i++;
               j++;                
            }
            if (this->iterator->line[i] != 'h') {
#ifdef DEBUG_ARCH
               printf (
                 "9. Format for entry %s in Events file %s is not supported\n",
                   this->iterator->line, this->archStr);
               printf ("Bad char is \'%c\' at position %d\n", 
                  this->iterator->line[i], i);
#endif
               return false;  //format error
            } 
            cAddr[j] = '\0';
            i++;
            lAddr = strtoul(cAddr, NULL, 16);

            //save data in register
            if (firstInSet) {
               e->registerSets[e->setCount].name = 
                  new char[strlen(tempName) + 1];
               memset(e->registerSets[e->setCount].name, 
                      '\0', strlen(tempName) + 1);
               strcpy(e->registerSets[e->setCount].name, tempName);
               e->registerSets[e->setCount].rType = iType;
               e->registerSets[e->setCount].number = iNum;
               e->registerSets[e->setCount].addr = lAddr;
               e->registerSets[e->setCount].next = NULL;
               firstInSet = false;
            } else {
               RegNode * addR = new RegNode;
               addR->name = new char[strlen(tempName) + 1];
               memset(addR->name, '\0', strlen(tempName) + 1);
               strcpy(addR->name, tempName);
               addR->rType = iType;
               addR->number = iNum;
               addR->addr = lAddr;
               addR->next = NULL;
               if (e->registerSets[e->setCount].next == NULL) {
                  e->registerSets[e->setCount].next = addR;
               } else {
                  RegNode * tempReg = e->registerSets[e->setCount].next;
                  if (tempReg != NULL) {
                     while (tempReg->next != NULL) {
                        tempReg = tempReg->next;
                     }
                     tempReg->next = addR;
                     tempReg = addR;
                  } else {
                     tempReg = addR;
                  }                              
               }
            }

            if ((this->iterator->line[i] == '\0') ||
                (i >= MAX_LINE_LEN)) {
#ifdef DEBUG_ARCH
              printf ("ARCH:parseEvent - reached end of line\n");
#endif
              break;
            } else {
               while (this->iterator->line[i] == '$') {
                  i++;
               }
            }
            j = 0;
         } //end while - parse pieces of a reg line
         //printf("\n");
         e->setCount++;

         //check to see if there are more arch nodes
         if(this->incrementIterator() != OK) {
#ifdef DEBUG_ARCH
            printf("3. ARCH:parseEvent call to incrementIterator failed\n"); 
#endif
            break;  //we don't want to continue if there are no more nodes
                    //in the Arch list
         };
         i = 0;  //reset i for starting next line
      } //end while - parse a register line
      //rv = true;
   } //end else

   return rv;
}



void 
Arch::resetIterator()
{
   this->iterator = this->head;
}


int 
Arch::incrementIterator()
{
   int rv;
   if (!this->isEmpty()) {
      if (this->iterator == NULL) {
         rv = E_NULL_NODE; 
      }else if (this->iterator->next == NULL) {
         //this->iterator = this->head;   //circles it back to head;
         //don't circle it back to the head; let user decide this
         // with reset option
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


int 
Arch::printIterator() const
{
   int rv;
   if (!this->isEmpty()) {
      if (this->iterator != NULL) {
         printf("\t%d\t%s\n", this->iterator->line);
         rv = OK;
      } else {
         printf("current iterator is null - try resetting it\n");
         rv = E_NULL_NODE;
      }
   } else {
      printf ("Event List is empty\n");
      rv = E_EMPTY_LIST;
   }
   return rv;
}


void 
Arch::printNodes() const
{
    if (!this->isEmpty()) {
        charNode * walker = this->head;
        while (walker != NULL) {
            printf ("%s\n", walker->line);
            walker = walker->next;
        }
    } else {
        printf ("Event List is empty\n");
    }
}

int
Arch::getCount() const
{
   return this->count;
}


bool
Arch::isEmpty() const
{
   bool rv;

   if (this->count > 0) {
      rv = false;
   } else {
      rv = true;
   }
   return rv;
}

bool
Arch::addLine(const char * data)
{
   char * parseStr;
   bool rv;

   parseStr = new char[strlen(data)+ 1];
   memset (parseStr, '\0', strlen(data) + 1);

   if (isprint(data[0])) {
      rv = this->addNode(data);
   }  else {
      rv = false;
   }
   return rv;
}

bool 
Arch::addNode (const char * data)
{
   bool rv;
   charNode * add = new charNode;
   if (add == NULL) { // no memory
      rv = false;
   } else {
      strcpy(add->line, data);
      add->next = NULL;
      this->count++;
      if (this->head == NULL) {
          this->head = add;
      } else {
          this->tail->next = add;
      }
      this->tail = add;
      rv = true;
   }
   return rv;
}

int
Arch::determineArchitecture(const char * archStr)
{
   int aid;
   if (strcmp(archStr, INTEL_15_6_STR) == 0  ) {
      aid = INTEL_15_6_ID;
      strcpy (this->archStr, archStr);
   } else if (strcmp (archStr, AMD_15_65_STR) == 0 ) {
      aid = AMD_15_65_ID;
      strcpy (this->archStr, archStr);
   } else {
     aid = NONSUPPORT_ID;
   } 
   return aid;
}

/**********************************
int main (int argc, char * argv[]) {
   char filename[MAX_ARCH_FILE_LEN];
   memset (filename, '\0', MAX_ARCH_FILE_LEN);
   strcpy(filename, "go beavers");
   Architecture arch (filename);
   return 0;
}
********************************/
