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


struct residue_item {
	int  divisor;
	int  left;
};

struct residue_item  residue_theorem(struct residue_item a, struct residue_item b) {

/*	printf("residue_theorem [%d,%d] [%d,%d]\n", a.divisor, a.left, b.divisor, b.left);*/
	int max_divisor = greates_common_divisor(a.divisor, b.divisor); 
	
	struct residue_item res;
	
	res.divisor = a.divisor * b.divisor / max_divisor; 

	int try = a.divisor + a.left;
	while (1) {
		if (try % b.divisor ==  b.left)break;
		try += a.divisor;
	}

	res.left = try % res.divisor;
/*	printf(" = [%d,%d]\n", res.divisor, res.left);*/
	return res;

}

void func(int cnt, 	struct residue_item *detail) {

	int i,j;
	if (cnt == 1) {
		printf("%d\n", detail[0].divisor + detail[0].left);
		return;
	} 

	
	struct residue_item res;
	res = residue_theorem(detail[0], detail[1]);

	for (i=2; i<cnt; i++) {
		res = residue_theorem(res, detail[i]);
	}

	
		printf("%d\n",res.left);
	return;
}

int main(int argc, char **argv) {

	int cnt[100];
	struct residue_item * detail[100];
	int index = 0;

	int i,j;
	for (;;) {
		if (scanf("%d", &cnt[index]) == EOF) {
			break;
		}
		detail[index] = malloc(sizeof(struct residue_item)*cnt[index]);
	
		for (i=0; i<cnt[index]; i++) {
			scanf("%d %d", &detail[index][i].divisor, &detail[index][i].left);
		}	
		index++;
	}
	
	for (i=0; i<index; i++) {
		func(cnt[i], detail[i]);
	}

	return 0;

}
