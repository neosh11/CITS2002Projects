#include <stdio.h>

#define foreach(i,x,y) for(int i = x; i < y; i++)
#define printfln(S, ...) printf(S, ##__VA_ARGS__); printf("\n");
#define eval(X) X

extern void lala(int x);