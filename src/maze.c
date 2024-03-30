#include <stdlib.h>
#include <stdio.h>
#include "maze.h"

void set_bit(int value, int n, uint16_t* v) {
	if (value != 0)
		v[n / BIT_TABLE_SIZE] |= 1 << (n % BIT_TABLE_SIZE);
	else
		v[n / BIT_TABLE_SIZE] &= ~(1 << (n % BIT_TABLE_SIZE));
}

int get_bit(int n, uint16_t* v) {
	return ((v[n / BIT_TABLE_SIZE] & (1 << (n % BIT_TABLE_SIZE))) != 0);
}

int is_wall(int a, int b, int x, int y, maze m) {
	int tmp;
	if (a == x && b == y)
		return 3;
	if (a >= m->col || x >= m->col || b >= m->row || y >= m->row)
		return 4;
	if (a == x) {
		if (y < b) {
			tmp = b;
			b = y;
			y = tmp;
		}
		if (y - b != 1)
			return 2;
		tmp = get_bit((m->col * 2 - 1) * b + x + m->col - 1, m->v);
		return tmp;
	}
	else if (b == y) {
		if (x < a) {
			tmp = a;
			a = x;
			x = tmp;
		}
		if (x - a != 1)
			return 2;
		tmp = get_bit((m->col* 2 - 1) * y + a, m->v);
		return tmp;
	}
	else
		return 2;
}

void free_maze(maze m) {
	if (m != NULL && m->v != NULL)
		free(m->v);
	if (m != NULL)
		free(m);
}

void print_maze(maze m) {
	int i, j, n = 0;
	for (i = 0; i < m->col * 2 + 1; i++)
		printf("X");
	printf("\nX ");
	for (i = 0; i < m->col - 1; i++) {
		if (get_bit(n, m->v) == 1)
			printf("X");
		else
			printf(" ");
		n++;
		printf(" ");

	}
	printf("X\n");
	for (i = 0; i < m->row - 1; i++){
		printf("X");
		for (j = 0; j < m->col; j++) {
			if(get_bit(n, m->v) == 1)
				printf("X");
			else
				printf(" ");
			n++;
			printf("X");
		}
		printf("\nX ");
		for (j = 0; j < m->col - 1; j++) {
			if (get_bit(n, m->v) == 1)
				printf("X");
			else
				printf(" ");
			n++;
			printf(" ");
		}
		printf("X\n");
	}
	for (i = 0; i < m->col * 2 + 1; i++)
		printf("X");
	printf("\n");
}
