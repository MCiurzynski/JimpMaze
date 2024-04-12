#ifndef FIND_PATH_H
#define FIND_PATH_H

#include "maze.h"

typedef struct path_struct {
	char *v;
	int steps;
	int n;
} *path;

path double_size(path);

void free_path(path);

path find_path(maze);

int bfs(uint16_t, uint16_t, uint16_t, uint16_t, maze);

void make_wall(maze, uint16_t*);

#endif
