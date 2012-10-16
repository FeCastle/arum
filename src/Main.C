/*
* file: Main.C
* purpose: Arum main program
* description: Use Arum to launch an application and collect performance 
*              statistics about that application.  Arum uses a loadable kernel
*              module to collect hardware performance counter data.  Arum uses
*              dynamic instrumentation to instrument the running application.
* status: 07-2007 - Current development is on collecting hardware performance
*         counter data.  Dynamic instrumentation of the running app is not yet
*         supported.
* 
* usage: Arum executable [options]
* 
* option summary:
*  -h | --events list     list of hardware counter events to collect; separate 
*                         event names by white space.
* 
*  -m | --mulitplex       This turns on time multiplexing for hardware counter
*                         collection.
* 
*  -f | --file filename   Read configuration settings from a file (i.e., the 
*                         counters to specify, etc.)
*/

#include <time.h>
#include <wait.h>
#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <getopt.h>

//#include "Timer.h"
//#include "GetTimeOfDay.h"
#include "ResourceUsage.h"
#include "Counters.h"
#include "Main.h"

/**
static int  parse_args( int argc, char* argv[], Main m);
static void print_args( int argc, char* argv[] );
**/

int
main( int argc, char* argv[], char* envp[] )
{
    Main mainMain;

    int i, rv, fd;
    if (mainMain.parse_args( argc, argv ) < 0) {
	mainMain.print_args( argc, argv );
	exit(-1);
    }
/****
#ifdef DEBUG_M    
    for (i=1; i<argc; i++){
       printf ("What's in argv[%d]? -- %s\n", i, argv[i]);
    }
    if (confFileFlag){
       printf ("Config file: %s\n", mainMain.configFile);
    }
    if (eventsListFlag) {
       printf ("Event list: %s\n", mainMain.eventsListStr);
    }
    if (mplexFlag) {
       printf ("Multiplex flag set\n");
    }
#endif
*****/

   Counters cntrs;
   if (eventsListFlag){
      cntrs = Counters (mainMain.eventsListStr);
      if ((fd = cntrs.openDevice() < 0)) {
         mainMain.perfCounters = false; 
         mainMain.error("Open device failed.\nPerformance counter monitoring will be disabled.");
      } else {
         mainMain.perfCounters = true;
      }
   } 

#ifdef DEBUG_M
   cntrs.printStats();
#endif

   //Timer* timer = new GetTimeOfDay(); 
   //double s = timer->seconds();

    pid_t child = fork();
    if (child == 0) {
        // child process
        // tell kernel which events to monitor
        if (mainMain.perfCounters){
           if ((rv = cntrs.writeCounters (fd, mplexFlag)) < 0) {
              mainMain.perfCounters = false;
              cntrs.closeDevice(fd);
              mainMain.error("Write to device failed.\nPerformance counter monitoring will be disabled."); 
           }
        }
	/**
           although it would seem correct to do this:
               execve( argv[1], argv+1, envp );
           we can't, because the use of getopt changes the positioning of 
           elements in argv, so we have this instead.
        **/
        execve( argv[argc -1], argv + (argc - 1), envp);
    } else {
        // parent process
	int status = 0;
	wait(&status);
    }
    //Stop Timers
    //double e = timer->seconds();
    ResourceUsage ru( ResourceUsage::CHILDREN );

    if (mainMain.perfCounters){    
       //Read Counters -- should do in a loop (until no more to read or error)
       if ((rv = cntrs.readCounters (fd)) < 0) {
          mainMain.perfCounters = false;
          mainMain.error("Read from device failed.\nCan't report performance counter results.");
       } else {
          #ifdef DEBUG_M
            printf ("Counters successfully read.\n");
          #endif
       }
       //Close Device
       rv = cntrs.closeDevice(fd);
    }

    printf("User time is %.1lf seconds\n", ru.user_time());
    printf("System time is %.1lf seconds\n", ru.sys_time());
    //printf("Elapsed time is %.1lf seconds\n", e-s);

    if (mainMain.perfCounters) {
       cntrs.printReport();
    }
    //delete timer;
}

// Main::parse_args()
// Purpose: parses command line arguments 
// Description: Uses getopt_long to parse the arguments and options; this
//     variant supports the GNU long option syntax as well as short options. 
//     Used man page example as guide.  The option structure for long options 
//     is defined in the constructor for class Main.
int 
Main::parse_args( int argc, char* argv[])
{
    /*
     Using getopt_long to parse the arguments and options; this
     variant supports the GNU long option syntax as well as
     short options. Used man page example as guide.  The option
     structure for long options is defined in class Main.
    */

    int c;
    int error = -1;

    if (argc < 2) {
	return error;
    }

    //read arguments and options
    while (1) {
       int option_index = 0;

       c = getopt_long (argc, argv, "h:mf:", long_options,
                        &option_index);
       if (c == -1) {
          break;
       } 
       switch (c) {
          case 0:
             //Long option encountered and a flag may have been set: 
             //config file, multiplex, or events
             if (long_options[option_index].flag != 0) {
                if (optarg) {
                  if (long_options[option_index].name == "file") {
                     if (optarg[0] == '-') {
                        printf ("Usage: must provide file name with --file\n");
                        return error;
                     }
                     if (strlen(optarg) <= MAX_FILENAME_LEN) {
                        strcpy(configFile, optarg);
                     } else {
                        printf 
                          ("Usage: file name length exceeds %d characters\n", 
                           MAX_FILENAME_LEN);
                        return error;
                     }
                  } else if (long_options[option_index].name == "events") {
                     if (optarg[0] == '-') {
                        printf 
                           ("Usage: must provide event list with --events\n");
                        return error;
                     }
                     if (strlen(optarg) <= MAX_EVENTS_LEN) {
                        strcpy(eventsListStr, optarg);
                     } else {
                        printf
                           ("Usage: events list length exceeds %d characters\n",
                            MAX_EVENTS_LEN);
                        return error;  
                     }
                  } else {
                  }
                } 
#ifdef DEBUG_M
                printf ("Flag set: option %s",
                        long_options[option_index].name);
                if (optarg) {
                  printf (" with arg %s", optarg);
                  if (long_options[option_index].name == "file") {
                     printf (" and file %s", configFile);
                  } else if (long_options[option_index].name == "events") {
                     printf (" and events %s", eventsListStr);
                  } else {
                  }
                }
                printf ("\n");
#endif
                break;
             }
#ifdef DEBUG_M
             printf ("Flag not set: likely error: option %s", 
                      long_options[option_index].name);
             if (optarg) {
               printf (" with arg %s", optarg);
             }
             printf ("\n");
#endif
             break;

          case 'h':
             //short option for hardware counter events
             if (optarg) {
                if (optarg[0] == '-') {
                   printf ("Usage: must provide event list with -h\n");
                   return error;
                }
                // Set flag and capture argument
                eventsListFlag = 1;
                if (strlen(optarg) <= MAX_EVENTS_LEN) {
                   strcpy(eventsListStr, optarg);
                } else {
                   printf ("Usage: events list length exceeds %d characters\n",
                           MAX_EVENTS_LEN);
                   return error;
                }
             } else {
                printf ("Usage: must provide event list with -h\n");
                return error;    
             }

#ifdef DEBUG_M
             printf ("short option h with argument '%s'\n", optarg);
#endif
             break;

          case 'm':
             //short option for time multiplexing of hw events
             // Set flag 
             mplexFlag = 1;
#ifdef DEBUG_M
             printf ("short option m\n");
#endif
             break;

          case 'f':
             //short option for config file
             if (optarg) {
                if (optarg[0] == '-') {
                   printf ("Usage: must provide file name with -f\n");
                   return error;
                }
                // Set flag and capture argument
                confFileFlag = 1;
                if (strlen(optarg) <= MAX_FILENAME_LEN) {
                   strcpy(configFile, optarg);
                } else {
                   printf ("Usage: file name length exceeds %d characters\n",
                           MAX_FILENAME_LEN);
                   return error;
                }
             } else {
                printf ("Usage: must provide file name with -f\n");
                return error;
             }


#ifdef DEBUG_M
             printf ("short option f with argument '%s'\n", optarg);
#endif
             break;  

          case '?':
             //If getopt_long encounters error it returns '?', and
             //prints and error message
             return error;

          default:
             printf ("Error: getopt returned character code 0%o ??\n", c);
             return error;
      }

    } //END_WHILE

    if (optind < argc) {
       // These are the remaining arguments, should be the executable that
       // Arum will launch
       printf ("non-option ARGV-elements: ");
       while (optind < argc){
          printf ("%s ", argv[optind++]);
       } 
       printf ("\n");
    }

    return 0;
}

// Main::print_args()
// Purpose: prints a usage statement 
// Description: prints a usage statement showing the command line syntax for
//              using Arum
void
Main::print_args( int argc, char* argv[] )
{
    printf("Usage: %s program [args...]\n", argv[0]);
}

// Main::Main()
// Purpose: Constructor
// Description: Allocates space for variables related to setting flags and
//              parsing command line arguments
Main::Main(){
   configFile = new char[MAX_FILENAME_LEN + 1];
   eventsListStr = new char [MAX_EVENTS_LEN + 1];
   perfCounters = false;

   long_options[0].name = "events";
   long_options[0].has_arg = 1;
   long_options[0].flag = &eventsListFlag;
   long_options[0].val = 1;  

   long_options[1].name = "multiplex";
   long_options[1].has_arg = 0;
   long_options[1].flag = &mplexFlag;
   long_options[1].val = 1;

   long_options[2].name = "file";
   long_options[2].has_arg = 1;
   long_options[2].flag = &confFileFlag;
   long_options[2].val = 1;

   long_options[3].name = 0;
   long_options[3].has_arg = 0;
   long_options[3].flag = 0;
   long_options[3].val = 0;
}

// Main::~Main()
// Purpose: Destructor
Main::~Main(){
}

// Main::error()
// Purpose: print error strings
void 
Main::error (const char * errorStr) {
   printf ("%s\n", errorStr);
}
