/*
* file: HwCtrTest.C
* purpose: user space program to test the device operations of 
*          the hardware counter module. 
* Description: By default, works with the device Arum, but if 
*              an argument is supplied, will test the basic 
*              operations for that device (assumes device is located under 
*              /dev).
* usage: test-module [deviceName]
*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

int main (int argc, char * argv[]) 
{
   int fd, rv;
   char * dev1 = "/dev/";
   char * device;

   if (argc > 2) {
      printf ("Usage: test-module [deviceName]\n");
      exit (-1);
   }
   if (argc == 1) {
      device = "/dev/Arum";
   } else {
      device = (char *) malloc((strlen(dev1) + strlen(argv[1]) + 1) * sizeof(char));
      strcpy (device, dev1);
      strcat (device, argv[1]);
   }
   printf ("Device to operate on is %s\n", device);
   printf ("Opening device ...\n");
   fd = open (device, O_RDWR);
   if (fd < 0) {
      printf ("Open failed.\n");
      exit (-1);
   } else {
      printf ("Open succeeded!\n");
   }

   printf ("Closing device ...\n");
   rv = close (fd);
   if (rv < 0) {
      printf ("Close failed!\n");
      exit (-1);
   }         

   printf ("Test program completed\n");  
   return 0;
}
