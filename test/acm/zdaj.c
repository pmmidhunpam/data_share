#include <stdio.h>
#include <stdlib.h>


int greates_common_divisor(int a, int b) {
	
	while (1) {
		if (a == 0)return b;
		if (b == 0)return a;

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
}


int common_multiple(int a, int b) {
	return a * b / greates_common_divisor(a, b);
}

void func(int cnt, 	int (*detail) [2]) {

	int i,j;
	int max_detail[2];
	max_detail[0] = 0;
	for (i=0; i<cnt; i++) {
		if (detail[i][0] > max_detail[0]) {
			max_detail[0] = detail[i][0];
			max_detail[1] = detail[i][1];
		}
	}

	int try = max_detail[0] + max_detail[1]; 
	for (j=0; j<cnt; j++) {
			if (max_detail[0] ==  detail[j][0])continue;
			int delta;
			int left = try % detail[j][0];
			if (left != detail[j][1]) {
				if (left > detail[j][1]) {
					delta =  detail[j][0] +  detail[j][1] - left;
				} else {
					delta = detail[j][1] - left;
				}
			
				int tmp = max_detail[0]-detail[j][0];	
				while (1) {
					int mod = delta % tmp;
					if (mod == 0)break;
					int num = (tmp - mod)/detail[j][0];
					if (num < 1)num=1;
					delta += num*detail[j][0];
				}
				/*
				printf("try = %d, add %d*(%d / (%d - %d))\n", try, max_detail[0],delta, max_detail[0], detail[j][0]);
				*/
				try += max_detail[0]*(delta / (max_detail[0]-detail[j][0]));
				max_detail[0] = common_multiple(max_detail[0], detail[j][0]);
			}		
	}
	printf("%d\n", try);
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
