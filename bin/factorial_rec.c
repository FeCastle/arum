#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

void loop() {
        int i;
        while (i < 100000000 )
                i++;
}
long fact_recursion(int n)
{
    int factorial;
        int i = 1;
        loop();
    if(n==0)
            return 1;
    else
                factorial = n*fact_recursion(n-1);
                return factorial;
}

int main( int argc, char* argv[]) {

        int num = 5;
        long factorial;
    printf("Factorial using recursion\n");
        factorial = fact_recursion(num);
        printf("%d! = %ld\n", num, factorial);
        return;
}

