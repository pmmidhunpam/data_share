#include <stdio.h>

int area_nr;
int employee_nr;

int *capacity_area;

int **employee_favor;

int seat_nr;
struct seat_info {
	int area_id;
	int employee_id;
	int favor;
};

struct seat_info *seats;

int tmp_employee[100];

int update_empty_employee(int seat_id, int empty_employee_id[100]) {
	int empty_employee_nr;
	int i,j;
	memset(empty_employee_id, 0, sizeof(empty_employee_id));
	memset(tmp_employee, 0, sizeof(tmp_employee));
	for (i=0; i<seat_id; i++) {
		tmp_employee[seats[i].employee_id] = 1;
	}
	for (i=0, j=0; i<employee_nr; i++) {
		if (tmp_employee[i] == 0) {
			empty_employee_id[j++] = i;
		}
	}
	empty_employee_nr = j;
	return empty_employee_nr;
}

void func(int seat_id) {
	int empty_employee_id[100];
	int empty_employee_nr;
	if (seat_id == seat_nr-1) {
		empty_employee_nr = update_empty_employee(seat_id, empty_employee_id);
		seats[seat_id].employee_id = empty_employee_id[0];
		seats[seat_id].favor = employee_favor[empty_employee_id[0]][seats[seat_id].area_id];
		int i;
		int sum_favor = 0;
		printf("----------------------------\n");
		for (i=0; i<seat_nr; i++) {
			printf("seat[%d], area = %d, employee = %d, favor = %d\n", i, seats[i].area_id, seats[i].employee_id, seats[i].favor);
			sum_favor += seats[i].favor;
		}
		printf("sum favor = %d\n", sum_favor);
		return;
	}

	empty_employee_nr = update_empty_employee(seat_id, empty_employee_id);
	int i;
	for (i=0; i<empty_employee_nr; i++) {
		seats[seat_id].employee_id = empty_employee_id[i];
		seats[seat_id].favor = employee_favor[seats[seat_id].employee_id][seats[seat_id].area_id];	
		func(seat_id+1);
	}

}

int main(int argc, char **argv) {
	
	int i,j;

	printf("input:\n");
	scanf("%d %d", &area_nr, &employee_nr);

	capacity_area = (int *)malloc(sizeof(int) * area_nr);
	for (i=0; i<area_nr; i++) {
		scanf("%d", &capacity_area[i]);
	}

	employee_favor = (int **)malloc(sizeof(int*) * employee_nr);
	for (i=0; i<employee_nr; i++) {
		employee_favor[i] = (int *)malloc(sizeof(int) * area_nr);
		for (j=0; j<area_nr; j++) {
			scanf("%d", &employee_favor[i][j]);
		}
	}

	printf("input success:\n");

	printf("area_nr = %d, employee_nr = %d\n", area_nr, employee_nr);
	printf("area capacity: \n");
	for (i=0; i<area_nr; i++) {
		printf("%d\t", capacity_area[i]);	
	}
	printf("\n");
	printf("employee favor: \n");
	for (i=0; i<employee_nr; i++) {
		printf("[%d] : ", i);
		for (j=0; j<area_nr; j++) {
			printf("%d\t", employee_favor[i][j]);
		}
		printf("\n");
	}


	seat_nr = 0;	
	for (i=0; i<area_nr; i++) {
		seat_nr += capacity_area[i];
	}
	
	seats = (struct seat_info *)malloc(seat_nr * sizeof(struct seat_info*));
	
	int seat_id = 0;
	for (i=0; i<area_nr; i++) {
		for (j=0; j<capacity_area[i]; j++) {
			seats[seat_id].area_id = i;
			seats[seat_id].employee_id = -1;
			seats[seat_id].favor = -1;
			seat_id++;
		}
	}
	func(0);
	
	return 0;
}
