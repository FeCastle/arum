#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cctype>
#include <string.h>
#include "Events.h"
#include "Arch.h"
using namespace std; 


int main(int argc, char * argv[])
{

   //char * name;
   //char temp[25];
   struct ArchId aid = {"GenuineIntel", 15, 6};

   //printf ("Hello World!\n");
   //cout << "Enter name...\n";
   //cin >> temp;
   //name = new char[strlen(temp) + 1];
   //strcpy (name, temp);
   //printf ("Hello %s!\n", name);
   Events events(&aid);
   events.printEvents();
   events.lookupEventByName("instructions_completed");
   events.lookupEventByName("instructions_on_vacation");
   events.lookupEventByName("global_power_events");
   return 0;
}


