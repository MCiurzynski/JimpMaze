#include <stdint.h>

#ifndef MAZE_H
#define MAZE_H

#define BIT_TABLE_SIZE 16

typedef struct mase_struct {
	uint16_t *v;
	uint16_t col;
	uint16_t row;
	uint16_t start_x;
	uint16_t start_y;
	char start_direction;
	uint16_t end_x;
	uint16_t end_y;
	char end_direction;
} *maze;

void set_bit(int, int, uint16_t*);

int get_bit(int, uint16_t*);

int is_wall(int, int, int, int, maze);

void free_maze(maze);

void print_maze(maze);

#endif
