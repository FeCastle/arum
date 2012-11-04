/* FunctionCalls.C
** 
**
**
*/
#include <stdio.h>
#include <stdlib.h>

int add(int x, int n);
int foo(int x, int n);

int main()
{
	int a, n;
	printf("Enter a large number, say over 5000\n");
	scanf("%d", &n);
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
