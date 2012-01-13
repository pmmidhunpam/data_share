#include <stdio.h>

int greates_common_divisor(int a, int b) {
		int res;
        while (1) {
                if (a == 0) {
					res = b;
					break;
				}
                if (b == 0) {
					res = a;
					break;
				}

                if (a >= b) {
                        int tmp = b;
                        b = a%b;
                        a = tmp;
                } else {
                        int tmp = a;
                        a = b%a;
                        b = tmp;
                }
        }
	printf("common divisor %d , %d = %d\n", a, b, res);
	return res;
}


int common_multiple(int a, int b) {
        return a * b / greates_common_divisor(a, b);
}


int common_multiple_s(int *p, int cnt) {
	
	int i,j;
	int res_div = greates_common_divisor(p[0], p[1]);
	
	for (i=2; i<cnt; i++) {
		res_div = greates_common_divisor(res_div, p[i]);
	}
	
	int res_mul = 1;
	for (i=0; i<cnt; i++) {
		res_mul *= p[i];
	}

	res_mul /= res_div;
	printf("common_multiple_s res = %d\n", res_mul);
}

int main(int argc, char **argv) {
	
	int a[7] = {9, 6, 7, 13, 24, 15, 10};
	common_multiple_s(a, 7);
	
	return 0;
}
