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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "BPatch.h"
#include "BPatch_thread.h"
#include "BPatch_addressSpace.h"
#include "BPatch_process.h"
#include "BPatch_binaryEdit.h"
#include "BPatch_function.h"
#include "BPatch_module.h"

using namespace Dyninst;
using namespace std;


#define VERBOSE

// Register this function as a callback, you cannot
// change the parameters to this function
static void printFuncCounts(BPatch_thread*, BPatch_exitType) {
  printf("\nMessage printed in printFuncCounts.\n");
}

vector< BPatch_function* >* getExecutableFuncs ( BPatch_image *image ) {
  vector<BPatch_function*> mainfuncs;
  image->findFunction("main", mainfuncs);
  BPatch_module *aOut = mainfuncs[0]->getModule();
  return aOut->getProcedures();
}

vector< const char * > * getFuncNames ( vector <BPatch_function*> *funcs ) {
  vector< const char* > * allFuncNames = new vector< const char* >();
  BPatch_Vector<const char*> funcNames;

#ifdef VERBOSE
  printf("Found functions:\n");
#endif

  for (unsigned fIdx=0; fIdx < (*funcs).size(); fIdx++) {
    BPatch_function *curfunc = (*funcs)[fIdx];
    curfunc->getNames(funcNames);
    assert(funcNames.size());
#ifdef VERBOSE
      printf("        %20s [0x%lx 0x%lx]\n",funcNames[0],
	     (long)curfunc->getBaseAddr(),
	     (long)curfunc->getBaseAddr() + curfunc->getSize());
#endif
    allFuncNames->push_back(funcNames[0]);
    funcNames.clear();
  }
  return allFuncNames;
}

void registerFunction(BPatch_addressSpace *handle, BPatch_image * appImage, std::vector<BPatch_function *> functions) {
  BPatch_Vector<const char*> funcNames;
  functions[0]->getNames(funcNames);
  printf("Start instrumenting function %s\n", funcNames[0]);
  std::vector<BPatch_point *> *entry_point = functions[0]->findPoint(BPatch_entry);
  std::vector<BPatch_point *> *exit_point = functions[0]->findPoint(BPatch_exit);

  // Create a snippet that calls printf with each effective address
  std::vector<BPatch_snippet *> printfArgs,timeArgs;
			
  //bpatch constant express for printing the performance measurement result
  BPatch_snippet *result = new BPatch_constExpr("\nTotal execution time for the given function is %u seconds\n\n\n");
  printfArgs.push_back(result);

  //look for the printf and time functions in the application image
  std::vector<BPatch_function *> printfFuncs;
  std::vector<BPatch_function *> getTime;
  appImage->findFunction("printf", printfFuncs);
  appImage->findFunction("time",getTime);

  //declare bpatch variables for begin time, end time and their difference as long
  BPatch_variableExpr *beginTime = handle->malloc(*(appImage->findType("long")));
  BPatch_variableExpr *endTime = handle->malloc(*(appImage->findType("long")));
  BPatch_variableExpr *diffTime = handle->malloc(*(appImage->findType("long")));

  //declare a constant express of zero, this value will be passed to the time function
  BPatch_snippet *time_zero = new BPatch_constExpr(0);
  timeArgs.push_back(time_zero);
  //create function expression for time function with argument 0
  BPatch_funcCallExpr getTimeCall(*(getTime[0]),timeArgs);

  //declare arithmetic expressions for assigning the begin and end time
  BPatch_arithExpr assignTime(BPatch_assign, *beginTime,getTimeCall);
  BPatch_arithExpr assignEndTime(BPatch_assign, *endTime,getTimeCall);
  //compute the difference of begin and end time
  BPatch_arithExpr assignDiffTime(BPatch_assign, *diffTime,BPatch_arithExpr(BPatch_minus,*endTime,*beginTime));
  //push the begin, end time and their difference to the print argument
  //printfArgs.push_back(beginTime); 
  //printfArgs.push_back(endTime); 
  printfArgs.push_back(diffTime);

  //delcare the print function expression
  BPatch_funcCallExpr printfCall(*(printfFuncs[0]), printfArgs);


  //insert the code of assigning the begin time at the entry point of the specified function
  handle->insertSnippet(assignTime,*entry_point);
  //insert the codes of assigning the end time, and compute the difference between begin time and end time to the exit point of the specified function
  //and then print the result
  //note that the order of inserting the codes matters, the first insertion codes will be executed last (like a stack) 
  handle->insertSnippet(printfCall, *exit_point);
  handle->insertSnippet(assignDiffTime, *exit_point);
  handle->insertSnippet(assignEndTime,*exit_point);
}

int main(int argc, char* argv[], char* envp[]) {
  char* name = argv[argc-1];

  printf("program name: %s\n", name);
	
  //create a new temporary file name for the given application binary after inserting the codes
  char new_name[256];
  strcpy(new_name,name);

  char* dyninstLibcEnv = NULL;
  dyninstLibcEnv = getenv("DYNINST_LIBC");
  if(NULL!=dyninstLibcEnv && 0!=strcmp(dyninstLibcEnv,"")) {
    BPatch bpatch;
    std::vector< const char* > *allFuncNames = NULL; // initialized in main
    std::vector< BPatch_function* > *allFuncs = NULL;// initialized in main

    // register printFuncCounts as an exit callback function
    bpatch.registerExitCallback( printFuncCounts );

    BPatch_addressSpace *handle = bpatch.openBinary(name);
    bool libFound = handle->loadLibrary(dyninstLibcEnv);
    if(libFound) {
#ifdef VERBOSE
      printf("LibC Library loaded\n");
#endif

      // get BPatch_image object
      BPatch_image * appImage = handle->getImage();

      // gather all functions in the executable, and their names
      allFuncs = getExecutableFuncs(appImage);
      allFuncNames = getFuncNames(allFuncs);
      //for (unsigned fIdx=0; fIdx < (*allFuncs).size(); fIdx++) {
      for (unsigned fIdx=0; fIdx < 1; fIdx++) {
	BPatch_function *curfunc = (*allFuncs)[fIdx];
	BPatch_Vector<const char*> funcNames;
	curfunc->getNames(funcNames);
	assert(funcNames.size());
#ifdef VERBOSE
	printf("Register function: %20s\n",funcNames[0]);
#endif

	const char* funcName = funcNames[0];
	std::vector<BPatch_function *> functions;
	bool funcfound = appImage->findFunction(funcName, functions);
	if(funcfound) {
	  registerFunction(handle, appImage, functions);

	  //BPatch_process *appProc = dynamic_cast<BPatch_process *>(handle);
	  //declare a bpatch_binaryEdit for the modified binary
	  BPatch_binaryEdit *appBin = dynamic_cast<BPatch_binaryEdit *>(handle);

	  //write back to the disk with the temporary name
	  if(appBin) {
	    strcpy(new_name,"arum_");
	    strcat(new_name,name);

	    appBin->writeFile(new_name);
	  }

#ifdef DEBUG_M
	  printf("We are at the end.\n");
#endif
	} else {	
	  printf("Warning : Unable to find function %s.Application performance monitoring will be disabled..\n",argv[argc-2]);
	}

	funcNames.clear();
      }

    } else {
	printf ("Warning: Please set DYNINST_LIBC enviroment variable to the Dyninst Libc library to measure the performance of executable. Application performance monitoring will be disabled.\n");
      }
  } else {
    printf ("Warning: Please set DYNINST_LIBC enviroment variable to the Dyninst Libc library to measure the performance of executable. Application performance monitoring will be disabled.\n");
  }

  pid_t child = fork();
  if(child ==0) {    
    //child process
#ifdef DEBUG_M
    printf("Executing: %s \n", name);
#endif

    int fd = open("out.txt", O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
    dup2(fd, 1); // redirect output to the file
    close(fd);

    // we execute the new application binary (that is the binary after insertion)
    execve(new_name, argv+(argc-1), envp);
  } else {
    // parent process
    int status = 0;
    wait(&status);
  } 

  //when done with Arum, we remove the temporary application binary
  if(strcmp(new_name, name) != 0) {
    if(remove(new_name) != 0) {
#ifdef DEBUG_M
      printf("Warning : Unable to remove temporary executable %s. Remove manually.\n",new_name);
#endif
    }	
  }
}

