/* Loop.C
**
** Basic test using a loop, expecting considerable execution time
**
*/
#include <stdio.h>
#include <stdlib.h>


int main( int argc, char* argv[])
{
	int i = 1; 
	while (i < 100000000 ) 
		i++; 
	printf("Basic Test using a long loop\n");
	return 0;
}



