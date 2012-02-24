#include <stdio.h>
#include <math.h>

int scores_nr;
int *scores;
int *tmp_scores;
int results = 0;

void do_it() {

	printf("\n******do it: ");
	int i;
	for (i=0; i<scores_nr; i++) {
		printf("%d ", tmp_scores[i]);
	}	
	printf("******\n");

	int rounds_nr = 0;
	int rounds_result[5][2];
	memset(rounds_result, 0, sizeof(rounds_result));
	
	int  score1, score2;

	score1 = score2 = 0;
	int add_score1 = 1;

	for (i=0; i<scores_nr; i++) {
		if (rounds_nr >= 5) {
			printf("\t can't play more than 5 rounds\n");
			return;
		}
		int score = tmp_scores[i];
		if (score == 11)
			score = 10;
		if (add_score1) {
			rounds_result[rounds_nr][0] += score;
		} else {
			rounds_result[rounds_nr][1] += score;
		}

		if (tmp_scores[i] != 10) {
			add_score1 = add_score1 == 1 ? 0 : 1;
		}

		if (rounds_result[rounds_nr][0] >= 21 || rounds_result[rounds_nr][1] >= 21) {
			if (abs(rounds_result[rounds_nr][0] - rounds_result[rounds_nr][1])  >= 2) {
				if (rounds_result[rounds_nr][0] > rounds_result[rounds_nr][1] > 0) {
					//player 1 win
					int delta = 0;
					if (rounds_result[rounds_nr][1] <= 19) {
						printf("\tround %d result, %d : %d\n", rounds_nr, 21, rounds_result[rounds_nr][1]);
						score1 = rounds_result[rounds_nr][0] - 21;
						score2 = 0;
						rounds_nr++;
						continue;
					} else {
						printf("\tround %d result, %d : %d\n", rounds_nr, rounds_result[rounds_nr][1]+2, rounds_result[rounds_nr][1]);
						score1 = rounds_result[rounds_nr][0] - rounds_result[rounds_nr][1] - 2;
						score2 = 0;
						rounds_nr++;
						continue;
					}		
				} else {
					//player 2 win
					if (rounds_result[rounds_nr][0] <= 19) {
						printf("\tround %d result, %d : %d\n", rounds_nr, rounds_result[rounds_nr][0], 21);
						score1 = 0;
						score2 = rounds_result[rounds_nr][1] - 21;
						rounds_nr++;
						continue;
					} else {
						printf("\tround %d result, %d : %d\n", rounds_nr, rounds_result[rounds_nr][0], rounds_result[rounds_nr][0]+2);
						score1 = 0;
						score2 = rounds_result[rounds_nr][1] - rounds_result[rounds_nr][0] - 2;
						rounds_nr++;
						continue;
					}
				}			
			} 
		}
	}

	if (rounds_nr < 5 &&(rounds_result[rounds_nr][0] != 0 || rounds_result[rounds_nr][1] != 0)) {
		printf("\tincomplete round %d result [%d : %d]\n", rounds_nr, rounds_result[rounds_nr][0], rounds_result[rounds_nr][1]);
		return;
	}
	
	results++;
}


void func(int score_id) {
	if (score_id == scores_nr) {
		//do it
		do_it();
	} else {
		if (scores[score_id] == -1) {
			//this x score and  subsquent score are combined socres
			tmp_scores[score_id] = 10;
			func(score_id+1);
			//this x score and  subsquent score are seperated socres
			tmp_scores[score_id] = 11;
			func(score_id+1);
		} else {
			tmp_scores[score_id] = scores[score_id];
			func(score_id+1);
		}
	}  
}


int main(int argc, char **argv) {


	FILE *fp = fdopen(0, "r");
	char line[1024];
	char *p;
	p = fgets(line, sizeof(line), fp);

	scores_nr = atoi(line);	
	
	fgets(line, sizeof(line), fp);
	scores = (int *)malloc(sizeof(int) * scores_nr);
	tmp_scores = (int *)malloc(sizeof(int) * scores_nr);
	int i;
	for (i=0; i<scores_nr; i++) {
		sscanf(p, "%c ", &scores[i]);
		p += 2;
		if (scores[i] == 'X' || scores[i] == 'x') {
			scores[i] = -1;
		} else if (scores[i] >= '0' && scores[i] <= '9') {
			switch(scores[i]) {
				case '0': scores[i]=0;break;
				case '1': scores[i]=1;break;
				case '2': scores[i]=2;break;
				case '3': scores[i]=3;break;
				case '4': scores[i]=4;break;
				case '5': scores[i]=5;break;
				case '6': scores[i]=6;break;
				case '7': scores[i]=7;break;
				case '8': scores[i]=8;break;
				case '9': scores[i]=9;break;
				default: break;
			}
		} else {
			printf("socre = %c input error\n", scores[i]);
			return 0;
		}
	}
	
	printf("socre nr = %d\n", scores_nr);
	for (i=0; i<scores_nr; i++) {
		printf("%d ", scores[i] );
	}
	printf("\n");


	func(0);
	
	
	if (results == 0) {
		printf("impossible\n");
	} else if (results == 1) {
		printf("yes, we got result\n");
	} else {
		printf("too much possibilities, can't determine\n");
	}
	return 0;
}
