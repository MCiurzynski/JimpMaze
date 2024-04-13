#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "maze.h"
#include "find_path.h"

int bfs(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, maze m) {
    int mv_x[4] = {0, 1, 0, -1};
    int mv_y[4] = {-1, 0, 1, 0};
    int i, len = 0, j, n = 0;
    int size = m->col * m->row;
    uint16_t visited[1024*1024] = {0};
    uint16_t next[1024*1024] = {0};
    visited[start_x + start_y * m->col] = 1;
    next[start_x + start_y * m->col] = 1;
    n++;
    while (n != 0) {
        for (i = 0; i < size; i++) {
                if (next[i] == 1 && visited[i] == 1) {
                    next[i] = 0;
                    n--;
                    for (j = 0; j < 4; j++) {
                        if (is_wall(i % m->col, i / m->col, i % m->col + mv_x[j], i / m->col + mv_y[j], m) == 0) { 
                           next[i % m->col + mv_x[j] + m->col * (i / m->col + mv_y[j])] = 1;
                            n++;
                        }
                    }
                }
        }
        for (i = 0; i < size; i++) {
            if (next[i] == 1)
                visited[i] = 1;
        }
        len++;
        if (visited[end_x + end_y * m->col] == 1) {
            return len;
        }
    }
    return -1;
}

int is_next_empty(uint16_t* v, int n) {
    int i;
    for (i = 0; i < n; i++)
        if (get_bit(i, v) == 1)
            return 0;
    return 1;
}

void make_wall(maze m, uint16_t* v);