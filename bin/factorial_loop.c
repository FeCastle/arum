#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

void loop() {
        int i;
        while (i < 100000000 )
                i++;
}
long fact_loop(int n)
{
        int c;
        long result = 1;

        int i = 1;
        loop();

        for (c = 1; c <= n; c++)
                result = result * c;

        return result;
}
int main( int argc, char* argv[]) {

        int num = 5;
        long factorial;
        printf("Factorial using loop \n");
        factorial = fact_loop(num);
        printf("%d! = %ld\n", num, factorial);
        return;
}

