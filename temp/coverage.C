#include <stdio.h>
#include "BPatch.h"
#include "BPatch_function.h"
using namespace Dyninst;
using namespace std;

// Global variables
BPatch bpatch;
std::vector< const char* > *allFuncNames = NULL; // initialized in main
std::vector< BPatch_function* > *allFuncs = NULL;// initialized in main


// Register this function as a callback, you cannot
// change the parameters to this function
static void printFuncCounts(BPatch_thread*, BPatch_exitType)
{
  printf("\nMessage printed in printFuncCounts.\n");
}


vector< BPatch_function* >* getExecutableFuncs ( BPatch_image *image )
{
    vector<BPatch_function*> mainfuncs;
    image->findFunction("main", mainfuncs);
    BPatch_module *aOut = mainfuncs[0]->getModule();
    return aOut->getProcedures();
}

vector< const char * > * getFuncNames ( vector <BPatch_function*> *funcs ) 
{
    vector< const char* > * allFuncNames = new vector< const char* >();
    BPatch_Vector<const char*> funcNames;
    for (unsigned fIdx=0; fIdx < (*funcs).size(); fIdx++) {
        BPatch_function *curfunc = (*funcs)[fIdx];
        curfunc->getNames(funcNames);
        assert(funcNames.size());
        if (1) {
            printf("Found func: %20s [0x%lx 0x%lx]\n",funcNames[0],
                   (long)curfunc->getBaseAddr(),
                   (long)curfunc->getBaseAddr() + curfunc->getSize());
        }
        allFuncNames->push_back(funcNames[0]);
        funcNames.clear();
    }
    return allFuncNames;
}


int main(int argc, char *argv[])
{
    /* 1. process control */

/*  1a. Check command line arguments                                      */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s prog_filename [args]\n",argv[0]);
        return 1;
    }

/*  1b. Create process, get BPatch_image object                           */
    BPatch_process * proc =  bpatch.processCreate( argv[1] , (const char**)(argv + 1) );
    assert ( proc ) ;
    // BPatch_image *image = proc->getImage();

    /* 2. create variables and initialize them at the entry point of main */ 

    /* 3. gather all functions in the executable, and their names */
    // Once you have defined the "image" object you can uncomment 
    // the following lines to initialize global vectors of all 
    // functions and their names
    // 
    // allFuncs = getExecutableFuncs( image );
    // allFuncNames = getFuncNames ( allFuncs );

    /* 4. instrument all function entry points with count snippets */ 

    /* 5. register printFuncCounts as an exit callback function */
    bpatch.registerExitCallback( printFuncCounts );
   
    /* 6. more process control */ 

    return 0;
}

