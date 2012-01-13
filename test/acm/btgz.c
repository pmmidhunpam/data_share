#include <stdio.h>


int combination(int subscript, int superscript) {
	if (subscript < superscript) {
		printf("error");
		return -1;
	}
	
	int subscript_tmp = subscript;
	int superscript_tmp = superscript;
	
	int ret = 1;

	int i;
	for (i=0; i<superscript; i++) {
		ret *= subscript_tmp;
		subscript_tmp--;
	}

	for (i=superscript; i>1; i--) {
		ret /= superscript_tmp;
		superscript_tmp--;
	}
	
	return ret;
}

static bt_delta = 0;
static bt_player_nr = 0;

struct bt_group_info {
	//fix
	int group_player_nr;
	int comb_sub_2;
	//dynamic
	int group_nr;
	int all_player_nr;
	int all_reduced_game_nr;
};
static struct bt_group_info* group_info = NULL;


void bt(int group_player_nr) {
	if (group_player_nr == 1) {
		return;
	}

	int all_player_nr = 0;
	int all_reduced_game_nr = 0;
	int i;
	for (i=bt_player_nr-1; i>group_player_nr; i--) {
		all_player_nr += group_info[i].all_player_nr;
		all_reduced_game_nr += group_info[i].all_reduced_game_nr;
	}
	
	int group_nr = (bt_player_nr-all_player_nr)/group_info[group_player_nr].group_player_nr;
	for (i=0; i<=group_nr; i++) {
		group_info[group_player_nr].group_nr = i;
		group_info[group_player_nr].all_player_nr = group_info[group_player_nr].group_player_nr * i;
		group_info[group_player_nr].all_reduced_game_nr = group_info[group_player_nr].comb_sub_2 * i;

		if (all_player_nr + group_info[group_player_nr].all_player_nr > bt_player_nr) {
			return;
		}

		if (group_info[group_player_nr].all_reduced_game_nr + all_reduced_game_nr < bt_delta) {
			bt(group_player_nr-1);
		} else if (group_info[group_player_nr].all_reduced_game_nr + all_reduced_game_nr == bt_delta) {
			printf("Yes, wo got resolution\n");
			int j;
			for (j=bt_player_nr-1; j>=group_player_nr; j--) {
				if (group_info[j].group_nr != 0)
				printf("group %d, group_nr = %d, all_player_nr = %d, all_reduced_game_nr = %d\n", j, group_info[j].group_nr, group_info[j].all_player_nr,  group_info[j].all_reduced_game_nr);
			}
		} 
	}

}


int main(int argc, char **argv) {

	int player_nr = atoi(argv[1]);
	int game_nr = atoi(argv[2]);

	printf("player_nr = %d, request game_nr  = %d\n", player_nr, game_nr);

	int max_game_nr = combination(player_nr, 2);
	printf("max_game_nr = %d\n", max_game_nr);
		

	if (game_nr < 0) {
		printf("game_nr can't less than zero\n");
	} else if (game_nr == 0) {
		printf("just only one group\n");
	} else if (game_nr == max_game_nr) {
		printf("just one group per player\n");
	} else if (game_nr > max_game_nr) {
		printf("game_nr is too large than max_game_nr\n");
	} else {
		printf("computing...\n");

		bt_delta = max_game_nr - game_nr;
		bt_player_nr = player_nr;

		group_info = (struct bt_group_info *)malloc(sizeof(struct bt_group_info*) * (player_nr));
		memset(group_info, 0, sizeof(struct bt_group_info*) * (player_nr));
		int i;
		for (i=2; i<player_nr; i++) {
			group_info[i].group_player_nr = i; 
			group_info[i].comb_sub_2 = combination(i, 2);
		}

		bt(player_nr-1);
	}

	return 0;
}
