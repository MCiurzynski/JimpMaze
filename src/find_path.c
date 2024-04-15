#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include "maze.h"
#include "find_path.h"

int bfs(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y, maze m) {
    int mv_x[4] = {0, 1, 0, -1};
    int mv_y[4] = {-1, 0, 1, 0};
    uint32_t n = 0, a, i;
    long long node;
    uint16_t *visited = calloc((m->row * m->col) / 16 + 1, sizeof(uint16_t));
    if (visited == NULL) {
        fprintf(stderr, "Nie udalo sie stworzyc tablicy odwiedzonych\n");
        return -1;
    }
    queue_head q = init_queue();
    if (q == NULL) {
        fprintf(stderr, "Nie udalo sie stworzyc kolejki\n");
        free(visited);
        return -1;
    }
    add_to_queue(start_x + start_y * m->col, q);
    set_bit(1, start_x + start_y * m->col, visited);
    while (q->first != NULL) {
        n++;
        a = get_from_queue(q);
        for (i = 0; i < 4; i++) {
            node = a % m->col + mv_x[i] + m->col * (a / m->col + mv_y[i]);
            if (is_wall(a % m->col, a / m->col, a % m->col + mv_x[i], a / m->col + mv_y[i], m) == 0 && get_bit(node, visited) == 0) {
                add_to_queue(node, q);
                set_bit(1, node, visited);
            }
        }
    }
    uint16_t *test = calloc((m->row * m->col) / 8 + 1, sizeof(uint16_t));
    printf("%ld\n", malloc_usable_size(test) + malloc_usable_size(visited) + malloc_usable_size(m->v));
    uint16_t t[131072] = {0};
    printf("%d %d\n", m->col, m->row);
    free(test);
    free(visited);
    free_queue(q);
    return n;
}

queue_head init_queue() {
    queue_head q = malloc(sizeof(*q));
    q->first = NULL;
    q->last = NULL;
    q->buff_first = NULL;
    q->buff_last = NULL;
    return q;
}

void free_queue(queue_head q) {
    queue_t tmp;
    while(q->buff_first != NULL) {
        tmp = q->buff_first;
        q->buff_first = tmp->next;
        free(tmp);
    }
    while(q->first != NULL) {
        tmp = q->first;
        q->first = tmp->next;
        free(tmp);
    }
    free(q);
}

void add_to_queue(int n, queue_head q) {
    queue_t tmp;
    if (q->buff_first == NULL)
        tmp = malloc(sizeof(struct queue));
    else {
        tmp = q->buff_first;
        q->buff_first = tmp->next;
        if (q->buff_first == NULL)
            q->buff_last = NULL;
    }
    tmp->n = n;
    tmp->next = NULL;
    if (q->first == NULL) {
        q->first = tmp;
        q->last = tmp;
    }
    else {
        q->last->next = tmp;
        q->last = tmp;
    }
}

uint32_t get_from_queue(queue_head q) {
    if (q->first == NULL)
        return -1;
    queue_t tmp;
    uint32_t n;
    tmp = q->first;
    if (tmp->next == NULL) {
        q->first = NULL;
        q->last = NULL;
    }
    else
        q->first = q->first->next;
    n = tmp->n;
    queue_t i;
    tmp->next = NULL;
    if (q->buff_first == NULL) {
        q->buff_first = tmp;
        q->buff_last = tmp;
    }
    else {
        q->buff_last->next = tmp;
        q->buff_last = tmp;
    }
    return n;
}

void make_wall(maze m, uint16_t* v);