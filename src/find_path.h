#ifndef FIND_PATH_H
#define FIND_PATH_H

int bfs(uint16_t, uint16_t, uint16_t, uint16_t, maze);

int is_next_empty(uint16_t*, int);

void make_wall(maze, uint16_t*);

#endif
