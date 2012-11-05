#if !defined( Arum_h )
#define Arum_h

#include <stdio.h>
#include <getopt.h>

//#define DEBUG_M
#define MAX_FILENAME_LEN 1024
#define MAX_EVENTS_LEN 1024 

static int confFileFlag = 0;
static int mplexFlag = 0;
static int eventsListFlag = 0;
static int resourceFlag = 0;  // -r option (or) --resources
static int programIndex = 0;  // Argv index of exec Program

class Main {
   public:
      static const long BodyColumnCount  = 60; 
      struct option long_options[4]; 
      char * configFile;
      char * eventsListStr;
      bool perfCounters;
      bool resourceCounters;

      Main();
      ~Main();
      int  parse_args( int argc, char* argv[]);
      void print_args( int argc, char* argv[]);
      void error (const char * errorStr);

   private:

};


#endif

