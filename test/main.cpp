#include <stdio.h>

extern int fun();
int main() {

   printf("fun = %0x\n", &fun);
   printf("fun = %0x\n", fun);
   return 0;
}
