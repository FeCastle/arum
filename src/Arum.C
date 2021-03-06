/*
* file: Arum.C
* purpose: Definition for class Main and contains the main() function.
* description: Use Arum to launch an application and collect performance 
*              statistics about that application.  Arum uses a loadable kernel
*              module to collect hardware performance counter data.  Arum uses
*              dynamic instrumentation to instrument the running application.
* status: 07-2007 - Current development is on collecting hardware performance
*         counter data.  Dynamic instrumentation of the running app is not yet
*         supported.
* 
* usage: Arum [arum options] executable [executable options]
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
*
*  -r | --resources       Display application resources used from getrusage
*                         such as page faults, block I/O, page faults, and 
*                         context switches.
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
#include <assert.h>

//#include "Timer.h"
//#include "GetTimeOfDay.h"
#include "ResourceUsage.h"
//#include "CpuId.h"
#include "Counters.h"
#include "Arum.h"

#include <binpro.h>

/**
static int  parse_args( int argc, char* argv[], Main m);
static void print_args( int argc, char* argv[] );
**/

int
main( int argc, char* argv[], char* envp[] )
{
    Main arum;
    int i, rv;
    if (arum.parse_args( argc, argv ) < 0) {
	arum.print_args( argc, argv );
	exit(-1);
    }
/****
#ifdef DEBUG_M    
    for (i=1; i<argc; i++){
       printf ("What's in argv[%d]? -- %s\n", i, argv[i]);
    }
    if (confFileFlag){
       printf ("Config file: %s\n", arum.configFile);
    }
    if (eventsListFlag) {
       printf ("Event list: %s\n", arum.eventsListStr);
    }
    if (mplexFlag) {
       printf ("Multiplex flag set\n");
    }
#endif
*****/

   Counters cntrs;                 // Declare a Counters instance

                                   // Initialize with event list,
                                   // debugging, and the mplexFlag.
   if (eventsListFlag){
      cntrs = Counters (arum.eventsListStr, true, (bool) mplexFlag);
      if (cntrs.isCompatible()) {  // check if event list is compatible
         if (cntrs.openDevice() < 0) {
            arum.perfCounters = false; 
            arum.error("Open device failed.\nPerformance counter monitoring will be disabled.");
         } else {
            arum.perfCounters = true;
         }
      } else {
	fprintf (stderr, "Warning:  inCompatible events for device\n");
         arum.perfCounters = false;
      }
   } 
#ifdef DEBUG_M
   if (cntrs.isCompatible()) {
      printf ("Compatible\n");
   } else {
      printf ("Not Compatible\n");
   }
#endif

   //Timer* timer = new GetTimeOfDay(); 
   //double s = timer->seconds();

    pid_t child = fork();
    if (child == 0) {
        // child process
        // tell kernel which events to monitor
        if (arum.perfCounters){
           if ((rv = cntrs.writeCounters ()) < 0) {
              arum.error("Write to device failed.\nPerformance counter monitoring will be disabled.");
              arum.perfCounters = false;
              cntrs.closeDevice();
           } else {
             #ifdef DEBUG_M
               printf ("Counters written successfully.\n");
             #endif
           }
        }
        
		// If this returns the program did not succeed
        execve( argv[programIndex], argv + (programIndex), envp);
		fprintf (stderr, "Error:  Unable to execute %s\n", argv[programIndex]);
		fprintf (stderr, "        May need full path?  (/bin/%s)\n", argv[programIndex]);
		return 128;
    } else {
        // parent process
	int status = 0;
	wait(&status);
    }
    //Stop Timers
    //double e = timer->seconds();
    ResourceUsage childUsage( ResourceUsage::CHILDREN );

    if (arum.perfCounters){    
       //Read Counters -- should do in a loop (until no more to read or error)
       if ((rv = cntrs.readCounters()) < 0) {
          arum.perfCounters = false;
          arum.error("Read from device failed.\nCan't report performance counter results.");
       } else {
          #ifdef DEBUG_M
            printf ("Counters successfully read.\n");
          #endif
       }
       //Close Device
       rv = cntrs.closeDevice();
    }

    printf("User time    %.4lf seconds\n", childUsage.user_time());
    printf("System time  %.4lf seconds\n", childUsage.sys_time());
    //printf("Elapsed time is %.1lf seconds\n", e-s);

    if (arum.resourceCounters) {
        printf("maxrss  - Max resident set size      %ld Kb\n",       childUsage.maxrss());
        printf("minflt  - Page faults without I/O    %ld\n",          childUsage.minor_fault());
        printf("majflt  - Page faults with I/O       %ld\n",          childUsage.major_fault());
        printf("inblock - File system input          %ld blocks\n",   childUsage.in_block());
        printf("oublock - File system output         %ld blocks\n",   childUsage.out_block());
        printf("nvcsw   - Voluntary context switch   %ld \n",         childUsage.vol_ctx_sw());
        printf("nivcsw  - Involuntary context switch %ld \n",         childUsage.invol_ctx_sw());
    }

    if (arum.perfCounters) {
       cntrs.printReport();
    }
    //delete timer;

    // binary probe
    printf("\n=========binary probe=========\n");
    instrumentProg(argc-1, argv+1, envp);
    struct ProfInfo profInfo;
    profInfo.invokenum = 0;
    profInfo.invoke = NULL;
    getProfInfo(&profInfo);
    assert(profInfo.invokenum != 0);
    profInfo.invoke = new struct InvokeInfo[profInfo.invokenum];
    assert(profInfo.invoke);
    getProfInfo(&profInfo);
    printf("\ncalling trace:\n");
    printf("  ustart   uend sstart   send  start    end function\n");
    printf("  ------ ------ ------ ------ ------ ------ --------\n");
    for(int i=0; i<profInfo.invokenum; i++) {
      printf("  % 6.2f % 6.2f % 6.2f % 6.2f % 6.2f % 6.2f %s\n", profInfo.invoke[i].ustart, profInfo.invoke[i].uend,
	     profInfo.invoke[i].sstart, profInfo.invoke[i].send, profInfo.invoke[i].start, profInfo.invoke[i].end, profInfo.invoke[i].funcname);
    }
    delete profInfo.invoke;
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

       // Add a '+' in front to quit processing when a NON option argument is present
       c = getopt_long (argc, argv, "+rh:mf:", long_options,
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
                  }
                  else {
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

          case 'r':
              resourceCounters = 1;
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
             //prints an error message
             return error;

          default:
             printf ("Error: getopt returned character code 0%o ??\n", c);
             return error;
      }

    } //END_WHILE

	if (resourceFlag) {
		// printf ("Long option --resources is set.\n");
    	resourceCounters = 1;
	}

    if (optind < argc) {
       // These are the remaining arguments, should be the executable that
       // Arum will launch
       programIndex = optind;
#ifdef DEBUG_M
       printf ("Program starts at index:  %d", programIndex);
       printf ("non-option ARGV-elements: ");
       while (optind < argc){
          printf ("%s - ", argv[optind++]);
       } 
       printf ("\n");
#endif
    }
    else {
        printf (" - EXEC option required\n");
        return error;
    }

    return 0;
}

// Main::print_args()
// Purpose: prints a usage statement 
// Description: prints a usage statement showing the command line syntax for
//              using Arum
// Todo: improve usage statement
void
Main::print_args( int argc, char* argv[] )
{
	fprintf(stderr, "Usage: %s [arum options] EXEC [EXEC opttions]\n"
	, argv[0]);
	fprintf(stderr, "Execute program EXEC [EXEC options] and display resource usage.\n");
	fprintf(stderr, "\n"); 
	fprintf(stderr, "Option summary:\n");
	fprintf(stderr, "  -h | --events <LIST>   LIST of hardware counter events to collect; separate\n");
	fprintf(stderr, "                         event names by white space.\n");
	fprintf(stderr, "\n"); 
	fprintf(stderr, "  -m | --mulitplex       This turns on time multiplexing for hardware counter\n");
	fprintf(stderr, "                         collection.\n");
	fprintf(stderr, "\n"); 
	fprintf(stderr, "  -f | --file <FILE>     Read configuration settings from <FILE> (i.e., the\n");
	fprintf(stderr, "                         counters to specify, etc.)\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "  -r | --resources       Display application resources used from getrusage\n");
	fprintf(stderr, "                         such as page faults, block I/O, page faults, and\n");
	fprintf(stderr, "                         context switches.\n");
	fprintf(stderr, "\n");
}

// Main::Main()
// Purpose: Constructor
// Description: Allocates space for variables related to setting flags and
//              parsing command line arguments
Main::Main(){
   configFile = new char[MAX_FILENAME_LEN + 1];
   eventsListStr = new char [MAX_EVENTS_LEN + 1];
   perfCounters = false;
   resourceCounters = false;

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

   long_options[3].name = "resources";
   long_options[3].has_arg = 0;
   long_options[3].flag = &resourceFlag;
   long_options[3].val = 1;
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
