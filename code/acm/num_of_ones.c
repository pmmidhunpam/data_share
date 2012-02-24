#include <stdio.h>
#include <stdlib.h>

int factorial(int n, int m = 0) {
	int i = 1;
	if (m != 0) i = n-m+1;
	int res = 1;
	for (; i<=n; i++) {
		res *= i;
	}
	return res;
}

int power_of_nine(int num) {
	if (num <= 0)return 1;
	int res = 1;
	for (int i=0; i<num; i++)
		res *= 9;
	return res;
}

int power_of_ten(int num) { 
	if (num <= 0)return 1;
	int res = 1;
	for (int i=0; i<num; i++)
		res *= 10;
	return res;
}

int cnt_onenums(int digit_cnt) {
	if (digit_cnt == 1)return 10;
	return power_of_ten(digit_cnt-1) + cnt_onenums(digit_cnt-1);
}

int cnt_ones(int digit_cnt) {
	int res = 0;
	for (int i=1; i<=digit_cnt; i++) {
		res += factorial(digit_cnt,i) / factorial(i) * power_of_nine(digit_cnt - i);
	}
	return res;
}

int fun(int num) {
	
	int digit_cnt = 1;
	int tmp1 = num;
	int max_digit = num;
	while (tmp1/10 > 0) {
		digit_cnt++;
		tmp1 = tmp1/10;
		max_digit = tmp1;
	}
	
	if (digit_cnt == 1) {
		if (max_digit == 0) {
			return 0;
		} else {
			return 1;
		}
	}
		
	int num_next = 0;
	int sub = 1;
	int tmp = digit_cnt;
	while(--tmp > 0) {
		sub *= 10;
	}
	num_next = num - sub*max_digit;

	printf("digit cnt = %d, ", digit_cnt);
	printf("max digit = %d, ", max_digit);
	printf("num next = %d\n", num_next);
	
	if (max_digit == 1) {
		return num_next + 1 + cnt_ones(digit_cnt-1) + fun(num_next);	
	}
	
	int res = power_of_ten(digit_cnt-1) + cnt_ones(digit_cnt-1);
	
	res += (max_digit - 1) * cnt_ones(digit_cnt-1);

	printf("res = %d\n", res);

	return res + fun(num_next);

}

int main(int argc, char **argv) {

	int num = atoi(argv[1]);
	printf("fun = %d\n", fun(num));
//	printf("factorial n = %d\n", factorial(num));
	return 0;
}


