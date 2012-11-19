#include <stdio.h>
#include <sys/types.h>
#include <unistd.h> 

long long_day()
{
	return 6151989;
}
void foo() {
   for (int i=0;i<900000;i++)
    printf(".");

}

int
main( int argc, char* argv[], char* envp[] )
{
   // for (long i=0; i < 200; i++) {
//	getpid();
  //  }
foo();
}
