#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct line {
	int from;
	int to;
	float rate;
	int active;
};
struct game {
	int player_cnt;
	int lines_cnt;
	struct line *lines;
};

struct game games[500];

int has_empty_line(struct game *game, struct line **defend_lines, int defend_lines_cnt, int player_nr) {
	int ret = 0;
	if (player_nr == game->player_cnt) {
		return 1;	
	}

	int i,j;
	for (i=0; i<game->lines_cnt; i++) {
		if (game->lines[i].from == player_nr) {
			int no_defend = 1;
			for (j=0; j<defend_lines_cnt; j++) {
				if (defend_lines[j] == &game->lines[i]) {
					no_defend = 0;
					break;
				}
			}
			if (no_defend && game->lines[i].active == 0) {
				game->lines[i].active = 1;
				if (has_empty_line(game, defend_lines, defend_lines_cnt, game->lines[i].to))
					return 1;
			}
		}
	}
	return ret;
}

void func(struct game *game) {

	struct line *defend_lines[400];
	int defend_player;
	float res = 0.0;

	for (defend_player=2; defend_player<=game->player_cnt; defend_player++) {
		int defend_lines_cnt = 0;
		int i;
		for (i=0; i<game->lines_cnt; i++) {
			if (game->lines[i].to == defend_player)
				defend_lines[defend_lines_cnt++] = &game->lines[i];
		}
		float defend_rate;
		if (defend_lines_cnt > 0) {
			for (i=0; i<defend_lines_cnt; i++) {			
				if (i==0) {
					defend_rate = 1-defend_lines[i]->rate;
				} else {
					defend_rate *= (1-defend_lines[i]->rate);
				}
			}
		}
		int ret = 0;
		if (defend_player == game->player_cnt) {
 			ret = 0;
		} else {
			 for (i=0; i<game->lines_cnt; i++) 
				game->lines[i].active = 0;
        	 ret = has_empty_line(game, defend_lines, defend_lines_cnt, 1);		
		}

		if (!ret) {
			float rate = pow(defend_rate, 1.0/defend_lines_cnt);
			if (rate > res) res = rate;
		}
	}
	if (res >= -0.000001 && res <= 0.000001) {
		printf("The opponent is too strong!\n");
	} else if (res == 1.0) {
		printf("The opponent is too week!\n");
	} else {
		printf("%f\n", res);
	}
		
}

int main(int argc, char **argv) {

	int games_nr = 0;
	int i;
	for (;;) {
		scanf("%d %d", &games[games_nr].player_cnt, &games[games_nr].lines_cnt);
		if (games[games_nr].player_cnt == 0 && games[games_nr].lines_cnt == 0) {
			break;
		}
		games[games_nr].lines = (struct line*)malloc(sizeof(struct line)*games[games_nr].lines_cnt);
		for (i=0; i<games[games_nr].lines_cnt; i++) {
			scanf("%d %d %f",  &games[games_nr].lines[i].from, &games[games_nr].lines[i].to, &games[games_nr].lines[i].rate);
/*	
		if (games[games_nr].lines[i].from > games[games_nr].lines[i].to)	
			*(int*)0 = 100;
*/
/*
		if (games[games_nr].lines[i].from > games[games_nr].lines[i].to) {
				int temp = games[games_nr].lines[i].from;
				games[games_nr].lines[i].from = games[games_nr].lines[i].to;
				games[games_nr].lines[i].to = temp;
		}
*/
		}
		games_nr++;
	}
	
	for (i=0; i<games_nr; i++) {
		func(&games[i]);
	}
/*
	 for (i=0; i<games[0].lines_cnt; i++) {
		printf("%d->%d, %d\n", games[0].lines[i].from, games[0].lines[i].to, games[0].lines[i].active);
	}	
*/
	return 0;
	
}
