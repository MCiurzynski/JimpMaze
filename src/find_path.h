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

int bfs(uint16_t, uint16_t, uint16_t, uint16_t, maze);

queue_head init_queue();

void free_queue(queue_head);

void add_to_queue(int, queue_head);

uint32_t get_from_queue(queue_head);

void make_wall(maze, uint16_t*);

#endif
