#include <stdio.h>
#include <stdlib.h>

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
	return res;
}


long long common_multiple(long long  a, long long b) {
        return a * b / greates_common_divisor(a, b);
}


long long common_multiple_s(int *p, int cnt) {
	
	int i,j;
	int res_div = greates_common_divisor(p[0], p[1]);
	
	for (i=2; i<cnt; i++) {
		res_div = greates_common_divisor(res_div, p[i]);
	}
	
	long long res_mul = 1;
	for (i=0; i<cnt; i++) {
		res_mul *= p[i];
	}

	res_mul /= res_div;
	printf("common_multiple_s res = %lld\n", res_mul);
}


void func(int cnt, 	int (*detail) [2]) {

	int i,j;
	int tmp[100];

	long long res = 0;
	for (i=0; i<cnt; i++) {
		long long res_mul;
		int index = 0;
		for (j=0; j<cnt; j++) {
			if (j != i) {
				tmp[index++] = detail[j][0];
			}
		}
		res_mul = common_multiple_s(tmp, cnt-1);

		long long key = res_mul;
		while (1) {
			printf(" %lld %%  %d = %lld\n", key, detail[i][0], key % detail[i][0]);
			if (key % detail[i][0] == 1) break;
			key += res_mul;
		}
		
		res += key*detail[i][1];
	
	}
	
	for (i=0; i<cnt; i++) {
		tmp[i] = detail[i][0];
	}
	long long all_mul = common_multiple_s(tmp, cnt);

	res = res % all_mul;
	
	printf("res = %lld\n", res);
	
	return;
}

int main(int argc, char **argv) {

	int cnt[100];
	int (*detail[100])[2];
	int index = 0;

	int i,j;
	for (;;) {
		if (scanf("%d", &cnt[index]) == EOF) {
			break;
		}
		detail[index] = malloc(sizeof(int [2])*cnt[index]);
	
		for (i=0; i<cnt[index]; i++) {
			scanf("%d %d", &detail[index][i][0], &detail[index][i][1]);
		}	
		index++;
	}
	
	for (i=0; i<index; i++) {
		func(cnt[i], detail[i]);
	}

	return 0;

}
#include <stdio.h>

