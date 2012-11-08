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
/**
static int  parse_args( int argc, char* argv[], Main m);
static void print_args( int argc, char* argv[] );
**/

#define DEBUG_M

int	main( int argc, char* argv[], char* envp[] )
{
	//Looking for environment variable DYNINST_LIBC before using     
	//Dyninst API.  It wouldn't work if it is not specified.
	char * dyninstLibcEnv = "";
	dyninstLibcEnv = getenv("DYNINST_LIBC");
	char * name = argv[argc-1];

	//create a new temporary file name for the given application binary after inserting the codes
	char *prefix = "arum_";
	char new_name[100];
	strcpy(new_name,name);

        char *funcName = argv[argc-2];
	
	//if the DYNINST_LIBC is found, then we insert code with the given application binary
	if ( dyninstLibcEnv != NULL && dyninstLibcEnv != "" )
	{
		//create a BPatch instance so that we can use the functions in the entire dyninst library
		BPatch bpatch;

		//create a handle of the given appliation binary by using the openBinary function
		BPatch_addressSpace *handle = bpatch.openBinary(name);
		
		//create a variable to store the result of loading the DYNINST_LIBC library
		bool libFound = handle->loadLibrary(dyninstLibcEnv);

		//if libFound is true, the library is loaded successfully
		if(libFound)
		{
#ifdef DEBUG_M
			printf("LibC Library loaded\n");
#endif

			//get the image of the application using getImage
			BPatch_image * appImage = handle->getImage();

			//declare a vector to hold the functions that we want to insert codes
			std::vector<BPatch_function *> functions;

			//look for the function with the specified name: argv[argc-2] in the application image
			bool funcfound = appImage->findFunction(funcName, functions);

			if(funcfound)
			{
 
			    printf("\n\nStart instrumenting function %s\n", funcName);
				//declare a vector to hold the entry points of the function
			    std::vector<BPatch_point *> *entry_point = functions[0]->findPoint(BPatch_entry);

				//declare a vector to hold the exit points of the function
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


				//BPatch_process *appProc = dynamic_cast<BPatch_process *>(handle);
				//declare a bpatch_binaryEdit for the modified binary
				BPatch_binaryEdit *appBin = dynamic_cast<BPatch_binaryEdit *>(handle);

				//write back to the disk with the temporary name
				if (appBin) 
				{
					strcpy (new_name,"");
					strcat(new_name,prefix);
					strcat(new_name,name);

					appBin->writeFile(new_name);
				}

#ifdef DEBUG_M
				printf("We are at the end.\n");
#endif
			}
			else
			{	

				printf("Warning : Unable to find function %s.Application performance monitoring will be disabled..\n",argv[argc-2]);
			}
		}
		else
		{
			printf ("Warning: Please set DYNINST_LIBC enviroment variable to the Dyninst Libc library to measure the performance of executable. Application performance monitoring will be disabled.\n");
		}
        }
        else
        {
                printf ("Warning: Please set DYNINST_LIBC enviroment variable to the Dyninst Libc library to measure the performance of executable. Application performance monitoring will be disabled.\n");
        }

	pid_t child = fork();
	if (child ==0) {    
		//child process
#ifdef DEBUG_M
		printf ("Executing: %s \n", argv[argc -1]);
#endif
		const char* name = argv[argc-1];

		//we execute the new application binary (that is the binary after insertion)
		//execve( new_name, argv + (argc - 1), envp);
		execve( new_name, argv + (argc - 1), envp);

	} else {
		// parent process
		int status = 0;
		wait(&status);
	} 

	//when done with Arum, we remove the temporary application binary
	if(strcmp(new_name, name) != 0)
	{
	        if( remove(new_name) != 0)
	        {
#ifdef DEBUG_M
		     printf("Warning : Unable to remove temporary executable %s. Remove manually.\n",new_name);
#endif
	        }	
        }
}

