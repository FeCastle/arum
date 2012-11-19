/* Finding Prime numbers */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{
    int number_of_elements = 100000,i=1,j,c, iCount=0;
    char ans;
    int prime[number_of_elements];
    memset(prime,0,number_of_elements);

    printf("Finding first %d prime numbers\n", number_of_elements);
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

    return 0;
}



