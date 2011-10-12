#include <stdio.h>
#include <unistd.h>


class A {
private:
	int a;
	int b;
public:
	A() {
	  a =1;
	  b=2;
	  pB = new B(this);
	}

	void funA() {
	  printf("a = %d, b = %d\n", a, b);
	  pB->funB();
	}

class B {
public:
	B(A *pa) {
		p = pa;
	}

	void funB() {
		printf("p-a = %d, p->b = %d\n", p->a, p->b);
	}
	A *p;
};

	B *pB;
};


int main() {

	A a;
	a.funA();
	return 0;

}
