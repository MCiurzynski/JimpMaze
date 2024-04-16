#ifndef FIND_PATH_H
#define FIND_PATH_H

#pragma pack(push, 1)
typedef struct queue {
	uint32_t n;
    struct queue* next;
} *queue_t;

typedef struct queue_head {
    queue_t first;
    queue_t last;
    queue_t buff_first;
    queue_t buff_last;
} *queue_head;
#pragma pack(pop)

int find_path(maze);

int find_path_to_bin(maze, char*);

int find_path_small(maze m);

int find_path_to_bin_small(maze, char*);

void path_step_to_bin(int, int, FILE*);

uint16_t *bfs_smaller_maze(maze);

void add_parent(long long, long long, uint16_t*, maze);

long long get_parent(long long, uint16_t*, maze);

queue_head init_queue();

void free_queue(queue_head);

void add_to_queue(int, queue_head);

uint32_t get_from_queue(queue_head);

#endif
