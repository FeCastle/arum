
/* Permutation.C
** USING RECURSION TO PRINT PERMUTATIONS ON AN ARRAY
**
**
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
int main()
{
    char str[] = "permutation";
    permute(str, 0, strlen(str));
    printf("various permutations of string done\n");
    return 0;
}

