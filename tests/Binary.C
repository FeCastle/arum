/* Binary.C
** Printing binary
**
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_bin(int n) {
        char bin[] = "00000000";
        int i;
        for (i=0; i<8; i++, n=n/2)
                if (n%2) bin[7-i] = '1';
//      printf("%s\n", bin);
}

int main () {
        char buffer[] = "HELLO";
        int len = strlen(buffer);
	int i;
	for (i=0;i<len;i++)
                print_bin(buffer[i]);
	printf("done\n");
        return 0;
}



