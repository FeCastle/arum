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
#include "binpro.h"

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

void printFuncNames ( vector <BPatch_function*> *funcs ) {
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
    funcNames.clear();
  }
  return;
}

void registerFunction(BPatch_addressSpace *handle, BPatch_image * appImage, BPatch_function * function) {
  vector<BPatch_snippet *> printfArgs, timeArgs_begin, timeArgs_end;
  vector<BPatch_function *> printfFuncs, timesFuncs;
  appImage->findFunction("printf", printfFuncs);
  appImage->findFunction("times", timesFuncs);

  BPatch_type* longType = appImage->findType("long");
  assert(longType);
  // Create tms type
  BPatch_Vector<char*> names;
  BPatch_Vector<BPatch_type*> types;
  names.push_back(const_cast<char*>("tms_utime"));
  names.push_back(const_cast<char*>("tms_stime"));
  names.push_back(const_cast<char*>("tms_cutime"));
  names.push_back(const_cast<char*>("tms_cstime"));
  types.push_back(longType);
  types.push_back(longType);
  types.push_back(longType);
  types.push_back(longType);
  BPatch_type* struct_tms = BPatch::bpatch->createStruct("tms", names, types);
  BPatch_type* lp_struct_tms = BPatch::bpatch->createPointer("lptms", struct_tms);

  BPatch_variableExpr* tms_buf_begin = handle->malloc(*struct_tms);
  BPatch_variableExpr* lp_tms_buf_begin = handle->malloc(*lp_struct_tms);
  BPatch_arithExpr* assignBeginTMSAdd = new BPatch_arithExpr(BPatch_assign, *lp_tms_buf_begin, BPatch_arithExpr(BPatch_addr, *tms_buf_begin));
  BPatch_variableExpr* tms_buf_end = handle->malloc(*struct_tms);
  BPatch_variableExpr* lp_tms_buf_end = handle->malloc(*lp_struct_tms);
  BPatch_arithExpr* assignEndTMSAdd = new BPatch_arithExpr(BPatch_assign, *lp_tms_buf_end, BPatch_arithExpr(BPatch_addr, *tms_buf_end));

  timeArgs_begin.push_back(lp_tms_buf_begin);
  //BPatch_snippet* time_zero = new BPatch_constExpr(0);
  //timeArgs.push_back(time_zero);
  BPatch_funcCallExpr timeCall_begin(*(timesFuncs[0]),timeArgs_begin);
  timeArgs_end.push_back(lp_tms_buf_end);
  BPatch_funcCallExpr timeCall_end(*(timesFuncs[0]),timeArgs_end);

  BPatch_variableExpr *beginTime = handle->malloc(*longType);
  BPatch_variableExpr *endTime = handle->malloc(*longType);

  BPatch_arithExpr* assignBeginTime = new BPatch_arithExpr(BPatch_assign, *beginTime, timeCall_begin);
  BPatch_arithExpr* assignEndTime = new BPatch_arithExpr(BPatch_assign, *endTime, timeCall_end);
  //assignDiffTime = new BPatch_arithExpr(BPatch_assign, *diffTime, BPatch_arithExpr(BPatch_minus,*endTime,*beginTime));

  BPatch_Vector<BPatch_variableExpr*>* fields_begin = tms_buf_begin->getComponents();
  assert(fields_begin && fields_begin->size()==4);
  assert(0 == strcmp((*fields_begin)[0]->getName(), "tms_utime"));
  assert(0 == strcmp((*fields_begin)[1]->getName(), "tms_stime"));
  BPatch_Vector<BPatch_variableExpr*>* fields_end = tms_buf_end->getComponents();
  assert(fields_end && fields_end->size()==4);
  assert(0 == strcmp((*fields_end)[0]->getName(), "tms_utime"));
  assert(0 == strcmp((*fields_end)[1]->getName(), "tms_stime"));

  //char funcName[64];
  BPatch_snippet *result = new BPatch_constExpr("\n<ARUM_TIMES> %p %jd %jd %jd %jd %jd %jd\n");
  printfArgs.push_back(result);
  //printfArgs.push_back(new BPatch_constExpr(function->getName(funcName, 64))); // %s
  printfArgs.push_back(new BPatch_originalAddressExpr()); // %p
  printfArgs.push_back((*fields_begin)[0]);
  printfArgs.push_back((*fields_end)[0]);
  printfArgs.push_back((*fields_begin)[1]);
  printfArgs.push_back((*fields_end)[1]);
  printfArgs.push_back(beginTime);
  printfArgs.push_back(endTime);
  BPatch_funcCallExpr* printfCall = new BPatch_funcCallExpr(*(printfFuncs[0]), printfArgs);

  vector<BPatch_point *> *entry_point = function->findPoint(BPatch_entry);
  vector<BPatch_point *> *exit_point = function->findPoint(BPatch_exit);

  //insert the code at the entry point
  handle->insertSnippet(*assignBeginTime,*entry_point);
  handle->insertSnippet(*assignBeginTMSAdd, *entry_point);
  //at the end. note that the order of inserting the codes matters, the first insertion codes will be executed last (like a stack) 
  handle->insertSnippet(*printfCall, *exit_point);
  handle->insertSnippet(*assignEndTime,*exit_point);
  handle->insertSnippet(*assignEndTMSAdd, *entry_point);
}

static int s_funcnum = 0;
static int s_invokenum = 0;
void instrumentProg(int argc, char* argv[], char* envp[]) {
  char* name = argv[0];

  printf("program name: %s\n", name);

  //create a new temporary file name for the given application binary after inserting the codes
  char new_name[256];
  strcpy(new_name,name);

  int fd = open("arum_temp.txt", O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);

  char* dyninstLibcEnv = NULL;
  dyninstLibcEnv = getenv("DYNINST_LIBC");
  if(NULL!=dyninstLibcEnv && 0!=strcmp(dyninstLibcEnv,"")) {
    BPatch bpatch;
    vector< BPatch_function* > *allFuncs = NULL;// initialized in main

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
      printFuncNames(allFuncs);
      for (unsigned fIdx=0; fIdx < (*allFuncs).size(); fIdx++) {
	BPatch_function *curfunc = (*allFuncs)[fIdx];
	BPatch_Vector<const char*> funcNames;
	curfunc->getNames(funcNames);
	assert(funcNames.size());
#ifdef VERBOSE
	printf("Register function: %20s\n",funcNames[0]);
#endif
	char tempstr[1024];
	sprintf(tempstr, "\n<ARUM_FUNCS> %s 0x%lx 0x%lx\n",funcNames[0],
	     (long)curfunc->getBaseAddr(),
	     (long)curfunc->getBaseAddr() + curfunc->getSize());
	write(fd, tempstr, strlen(tempstr));

	registerFunction(handle, appImage, curfunc);

	funcNames.clear();
      }

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

    dup2(fd, 1); // redirect output to the file
    close(fd);

    // we execute the new application binary (that is the binary after insertion)
    execve(new_name, argv, envp);
  } else {
    // parent process
    close(fd);
    int status = 0;
    wait(&status);
  } 

  // when done with Arum, we remove the temporary application binary
  if(strcmp(new_name, name) != 0) {
    if(remove(new_name) != 0) {
#ifdef DEBUG_M
      printf("Warning : Unable to remove temporary executable %s. Remove manually.\n",new_name);
#endif
    }	
  }

  // filter
  FILE* infile = fopen("arum_temp.txt", "r+");
  if(infile == NULL) {
    printf("failed to open file arum_temp.txt\n");
    return;
  }
  FILE* outfile = fopen("arum.txt", "w+");
  if(outfile == NULL) {
    printf("failed to open file arum.txt\n");
    return;
  }

  s_funcnum = 0;
  s_invokenum = 0;
  const char* pattern1 = "<ARUM_FUNCS>";
  const char* pattern2 = "<ARUM_TIMES>";
  char buf[256];
  while(NULL != fgets(buf, 128, infile)) {
    if(strlen(buf) >= strlen(pattern1)) {
      if(0 == memcmp(buf, pattern1, strlen(pattern1))) {
	fprintf(outfile, "%s", buf);
	s_funcnum++;
      } else if(0 == memcmp(buf, pattern2, strlen(pattern2))) {
	fprintf(outfile, "%s", buf);
	s_invokenum++;
      }
    }
  }

  fclose(outfile);
  fclose(infile);

  // delete the output file
  remove("arum_temp.txt");
}

struct FuncInfo {
  char funcname[128];
  long startadd;
  long endadd;
};
void getProfInfo(struct ProfInfo* profInfo) {
  assert(profInfo != NULL);
  profInfo->invokenum = s_invokenum;
  if(profInfo->invoke == NULL) {
    return;
  }

  struct FuncInfo* funcInfo = new struct FuncInfo[s_funcnum];
  assert(funcInfo != NULL);

  // filter
  FILE* infile = fopen("arum.txt", "r+");
  if(infile == NULL) {
    printf("failed to open file arum.txt\n");
    return;
  }

  int funcind = 0;
  int invokeind = 0;
  const char* pattern1 = "<ARUM_FUNCS>";
  const char* pattern2 = "<ARUM_TIMES>";
  char buf[256];
  while(NULL != fgets(buf, 128, infile)) {
    if(strlen(buf) >= strlen(pattern1)) {
      if(0 == memcmp(buf, pattern1, strlen(pattern1))) {
	sscanf(buf, "<ARUM_FUNCS> %s 0x%lx 0x%lx\n", funcInfo[funcind].funcname, &(funcInfo[funcind].startadd), &(funcInfo[funcind].endadd));
	funcind++;
      } else if(0 == memcmp(buf, pattern2, strlen(pattern2))) {
	int i;
	void* add;
	clock_t ustart;
	clock_t uend;
	clock_t sstart;
	clock_t send;
	clock_t start;
	clock_t end;
	sscanf(buf, "<ARUM_TIMES> %p %jd %jd %jd %jd %jd %jd\n", &add, &ustart, &uend, &sstart, &send, &start, &end);
	for(i=0; i<s_funcnum; i++) {
	  if((long)add>=funcInfo[i].startadd && (long)add<=funcInfo[i].endadd) {
	    break;
	  }
	}
	assert(i != s_funcnum);
	strcpy(profInfo->invoke[invokeind].funcname, funcInfo[i].funcname);
	profInfo->invoke[invokeind].ustart = ustart * 1.0 / sysconf(_SC_CLK_TCK);
	profInfo->invoke[invokeind].uend = uend * 1.0 / sysconf(_SC_CLK_TCK);
	profInfo->invoke[invokeind].sstart = sstart * 1.0 / sysconf(_SC_CLK_TCK);
	profInfo->invoke[invokeind].send = send * 1.0 / sysconf(_SC_CLK_TCK);
	profInfo->invoke[invokeind].start = start * 1.0 / sysconf(_SC_CLK_TCK);
	profInfo->invoke[invokeind].end = end * 1.0 / sysconf(_SC_CLK_TCK);
	invokeind++;
      }
    }
  }

  fclose(infile);

  remove("arum.txt");

  delete funcInfo;
}

/*
int main(int argc, char* argv[], char* envp[]) {
  instrumentProg(argc-1, argv+1, envp);	
  return 0;
}
*/
