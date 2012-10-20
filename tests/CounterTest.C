/*
 * CounterTest
 */

#include "Counters.h"
#include "CpuId.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int
main(int ac, char*av[])
{
    const char * theEventStr = "c1 c2 c3";
    printf ("Test Counters Class and CPUId instruction.\n");
    //CpuId x;
    Counters cntrs;
    cntrs = Counters (theEventStr, true, false);
    if (cntrs.isCompatible()) {

       printf("The specified events can be counted\n"); 
    } else {
       printf 
         ("The specified event list is incompatible with the architecture\n");
    }
    printf ("\nEnd of Test.\n");
}

