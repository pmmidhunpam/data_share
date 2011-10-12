#include <stdio.h>
#include <stdlib.h>

struct A {
  int len;
  int data[0];
};

struct B {
  char val1:4;
  char val2:5;
  char val3:3;
};

struct C {
  short s1;
  char c1;
  short s2;
  short s3;
  char c2;
  int i;
};

void fun(int a) {
  int i = 888;
  i = a;
  return;
}
int main(int argc, char **argv) {
/*
  struct A *p;
  p = (struct A*)malloc(sizeof(struct A) + 100*sizeof(int));
  p->len = 100;
  int i;
  for (i=0; i<100; i++) {
    p->data[i] = i;
  }
  for (i=0; i<100; i++) {
    printf("%d ", p->data[i]);
  }
*/
  //fun(0);
  //
  char buf[10000][10000];
  return 0;
}
