#include <stdio.h>

int player_nr;
int game_nr;

int arr[100];



void judge(int depth) {
	int res = 0;
	int i,j,k;
	for (i=0; i<depth; i++) {
		for (j=i+1; j<=depth; j++) {
			res += arr[i]*arr[j];
		}
	}
	if (res == game_nr) {
		printf("ok: \n");
		for (i=0; i<=depth; i++) {
			printf("%d ", arr[i]);
		}
		printf("\n");
	}
}

void func(int depth, int num, int max) {

	if (num == 1) {
		arr[depth] = 1;
		judge(depth);		
	} else if (num == 0) {
		judge(depth-1);
	}

	int i;
	for (i=1; i<=max; i++) {
		arr[depth] = i;
		func(depth+1, num-i, max<num-i?max:num-i);
	}
}


int main(int argc, char **argv) {

	player_nr = atoi(argv[1]);
	game_nr = atoi(argv[2]);

	printf("player_nr = %d, request game_nr  = %d\n", player_nr, game_nr);


	func(0, player_nr, player_nr);

	return 0;
}
