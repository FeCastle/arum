/* Arum Benchmark Application*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include  <fcntl.h>
#include <time.h>

//1. Recursion - factorial calculation
//2. factorial calculation using loop - no recursion
//3. prime number generation
//4. writes to a file
//5. Permuatations of a string using recursion
//6. difftime() - to calculate time used by a loop using difftime(), this function is fake and never gets called.

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

void fn_Primes(int number_of_elements)
{
        int i=1,j,c, iCount=0;
    char ans;
    int prime[number_of_elements];
    memset(prime,0,number_of_elements);

    while(i<=number_of_elements)
        {
            c=0;
            for(j=1;j<=i;j++)
            {
                if(i%j==0)
                c++;
            }
            if(c==2){
                prime[iCount] = i;
                iCount++;
            }
            i++;
        }

    printf("Total %d primes found\n", iCount);

#ifdef DEBUG
    printf("Total %d primes found, print all (y/n)? ", iCount);
    scanf("%c", &ans);
    if (ans=='y') {
        for (i=0; i< iCount; i++) {
        printf("%d, ", prime[i]);
        }
        printf("\n");
    }
#endif /* DEBUG */

}
void write_file() {
        int i, n = 10;

        loop ();
        for(i=0; i< n; i++) {
                FILE * pFile;
                pFile = fopen ( "sample.txt" , "w" );
                if (pFile==NULL) {
                        fputs ("File error",stderr);
                        exit (1);
                }
                fwrite(&i,1, sizeof(i), pFile);
                fclose(pFile);
                remove("sample.txt");

                }
        printf("write %d numbers to file\n", n);

}
void swap(char* src, char* dst)
{
    char ch = *dst;
    *dst = *src;
    *src = ch;
}

/* permute [set[begin], set[end]) */
int permute(char* set, int begin, int end)
{
    int i;
    int range = end - begin;
    if (range == 1) {
        //printf("set: %s\n", set);
    } else {
        for(i=0; i<range; i++) {
            swap(&set[begin], &set[begin+i]);
            permute(set, begin+1, end);
            swap(&set[begin], &set[begin+i]);       /* set back */
           }
    }
     return 0;

}
void fake_difftime() {
        time_t start,end;
        volatile long unsigned counter;

        start = time(NULL);

        for(counter = 0; counter < 500000000; counter++)
                        ; /* Do nothing, just loop */

        end = time(NULL);
        printf("The loop used %f seconds.\n", difftime(end, start));
}

int main()
{
        int num = 5;
        int number_of_elements = 50000;
        long factorial;
    printf("Factorial using recursion\n");
        factorial = fact_recursion(num);
        printf("%d! = %ld\n", num, factorial);

        printf("Factorial using loop, no recursion\n");
        factorial = fact_loop(num);
        printf("%d! = %ld\n", num, factorial);

        // Generating prime numbers
    printf("Finding first %d prime numbers\n", number_of_elements);
        fn_Primes(number_of_elements);

        write_file();

        char str[] = "permutation";
    permute(str, 0, strlen(str));
    printf("various permutations of string done\n");

        return 0;
}


