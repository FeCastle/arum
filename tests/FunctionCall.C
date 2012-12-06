/* FunctionCalls.C
**
**
**
*/
#include <stdio.h>
#include <stdlib.h>

int add(int x, int n);
int foo(int x, int n);

void loop() {
        int i;
        while (i < 100000000 )
                i++;
}
int main()
{
        int a, n;
        n = 6000;
        loop();
        loop();
        a = foo(0,n);
        printf("%d\n", add(a,n));
        return 0;
}
int add(int x, int n)
{
        int i;
        for (i = 1; i <= n; i++)
                x += i;
        return x;
}
int foo(int x, int n)
{
        int i;
        for (i = 1; i <= n; i++)
                x += add(i,n);
        return x;
}

