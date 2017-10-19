#include <stdio.h>

#define foreach(i,x,y) for(int i = x; i < y; i++)
#define eval(X) X
#define progArgs(...) {__VA_ARGS__, NULL} 

extern void lala(int x);
extern char **argumentsArray(char ** array);
