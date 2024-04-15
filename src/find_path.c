#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include "maze.h"
#include "find_path.h"

int find_path(maze m) {
    uint16_t* parents = bfs_smaller_maze(m);
    if (parents == NULL) {
        fprintf(stderr, "Nie udalo sie znalezc sciezki\n");
        return -1;
    }
    int direction, end_directon;
    switch (m->start_direction) {
        case 'N':
            direction = 2;
            break;
        case 'E':
            direction = 3;
            break;
        case 'S':
            direction = 0;
            break;
        case 'W':
            direction = 1;
            break;
    }
    switch (m->end_direction) {
        case 'N':
            end_directon = 2;
            break;
        case 'E':
            end_directon = 3;
            break;
        case 'S':
            end_directon = 0;
            break;
        case 'W':
            end_directon = 1;
            break;
    }
    int next, len = 0;
    int node = m->start_x + m->col * m->start_y;
    printf("START\n");
    while (node != m->end_x + m->col * m->end_y) {
        next = get_parent(node, parents, m);
        if (next - node == -m->col) {
            if (direction == 0) {
                len++;
            }
            else {
                if (len != 0)
                    printf("FORWARD %d\n", len);
                len = 1;
                if (direction == 3) {
                    printf("TURNRIGHT\n");
                }
                else {
                    printf("TURNLEFT\n");
                }
                direction = 0;
            }
        }
        if (next - node == 1) {
            if (direction == 1) {
                len++;
            }
            else {
                if (len != 0)
                    printf("FORWARD %d\n", len);
                len = 1;
                if (direction == 0) {
                    printf("TURNRIGHT\n");
                }
                else {
                    printf("TURNLEFT\n");
                }
                direction = 1;
            }
        }
        if (next - node == m->col) {
            if (direction == 2) {
                len++;
            }
            else {
                if (len != 0)
                    printf("FORWARD %d\n", len);
                len = 1;
                if (direction == 1) {
                    printf("TURNRIGHT\n");
                }
                else {
                    printf("TURNLEFT\n");
                }
                direction = 2;
            }
        }
        if (next - node == -1) {
            if (direction == 3) {
                len++;
            }
            else {
                if (len != 0)
                    printf("FORWARD %d\n", len);
                len = 1;
                if (direction == 2) {
                    printf("TURNRIGHT\n");
                }
                else {
                    printf("TURNLEFT\n");
                }
                direction = 3;
            }
        }
        node = next;
    }
    if (len != 0)
        printf("FORWARD %d\n", len);
    if (end_directon - direction == -1) 
        printf("TURNRIGHT\n");
    if (end_directon - direction == 1)
        printf("TURNLEFT\n");
    printf("STOP\n");
    free(parents);
    return 0;
}

int find_path_to_bin(maze m, char* bin_file) {
    FILE* f = fopen(bin_file, "rb+");
    uint32_t offset_to_solution, is_offset;
    uint32_t steps;
    int steps_offset;
    int next, node, len, direction;
    if (f == NULL) {
        fprintf(stderr, "Nie udalo sie otworzyc pliku binarnego do zapisu sciezki\n");
        return -1;
    }
    fseek(f, 0, SEEK_END);
    offset_to_solution = ftell(f) - 4;
    fseek(f, 33, SEEK_SET);
    fread(&is_offset, sizeof is_offset, 1, f);
    if (is_offset != 0) {
        fprintf(stderr, "W pliku binarnym jest juz zapisana sciezka\n");
        return -2;
    }
    fseek(f, 33, SEEK_SET);
    fwrite(&offset_to_solution, sizeof offset_to_solution, 1, f);
    fseek(f, 0, SEEK_END);
    steps_offset = ftell(f);
    steps = 0;
    fwrite(&steps, sizeof steps, 1, f);
    uint16_t* parents = bfs_smaller_maze(m);
    if (parents == NULL) {
        fprintf(stderr, "Nie udalo sie znalezc sciezki\n");
        return -1;
    }
    node = m->start_x + m->col * m->start_y;
    next = get_parent(node, parents, m);
    if (next - node == -m->col)
        direction = 0;
    if (next - node == 1)
        direction = 1;
    if (next - node == m->col)
        direction = 2;
    if (next - node == -1)
        direction = 3;
    node = next;
    len = 1;
    while (next != m->end_x + m->col * m->end_y) {
        next = get_parent(node, parents, m);
        if (next - node == -m->col) {
            if (direction == 0) {
                len++;
                if (len == 256) {
                    path_step_to_bin(len, direction, f);
                    steps++;
                    len = 0;
                }
            }
            else {
                path_step_to_bin(len, direction, f);
                len = 1;
                direction = 0;
            }
        }
        if (next - node == 1) {
            if (direction == 1) {
                len++;
                if (len == 256) {
                    path_step_to_bin(len, direction, f);
                    steps++;
                    len = 0;
                }
            }
            else {
                path_step_to_bin(len, direction, f);
                steps++;
                len = 1;
                direction = 1;
            }
        }
        if (next - node == m->col) {
            if (direction == 2) {
                len++;
                if (len == 256) {
                    path_step_to_bin(len, direction, f);
                    steps++;
                    len = 0;
                }
            }
            else {
                path_step_to_bin(len, direction, f);
                steps++;
                len = 1;
                direction = 2;
            }
        }
        if (next - node == -1) {
            if (direction == 3) {
                len++;
                if (len == 256) {
                    path_step_to_bin(len, direction, f);
                    steps++;
                    len = 0;
                }
            }
            else {
                path_step_to_bin(len, direction, f);
                steps++;
                len = 1;
                direction = 3;
            }
        }
        node = next;
    }
    if (len != 0)
        path_step_to_bin(len, direction, f);
    fseek(f, steps_offset, SEEK_SET);
    fwrite(&steps, sizeof steps, 1, f);
    fclose(f);
    return 0;
}

void path_step_to_bin(int len, int direction, FILE* f) {
    uint8_t tmp = len - 1, c;
    switch (direction)
    {
    case 0:
        c = 'N';
        break;
    case 1:
        c = 'E';
        break;
    case 2:
        c = 'S';
        break;
    case 3:
        c = 'W';
        break;
    }
    fwrite(&c, 1, 1, f);
    fwrite(&tmp,1, 1, f);
}

uint16_t* bfs_smaller_maze(maze m) {
    uint16_t start_x = m->end_x;
    uint16_t start_y = m->end_y;
    int mv_x[4] = {0, 1, 0, -1};
    int mv_y[4] = {1, 0, -1, 0};
    uint32_t n = 0, a, i;
    long long node;
    int is_path;
    uint16_t *visited = calloc((m->row * m->col) / 16 + 1, sizeof(uint16_t));
    if (visited == NULL) {
        fprintf(stderr, "Nie udalo sie stworzyc tablicy odwiedzonych\n");
        return NULL;
    }
    uint16_t *parents = calloc((m->row * m->col) / 8 + 1, sizeof(uint16_t));
    if (parents == NULL) {
        fprintf(stderr, "Nie udalo sie stworzyc tablicy poprzednikow\n");
        free(visited);
        return NULL;
    }
    queue_head q = init_queue();
    if (q == NULL) {
        fprintf(stderr, "Nie udalo sie stworzyc kolejki\n");
        free(visited);
        free(parents);
        return NULL;
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
                add_parent(node, a, parents, m);
                if (node == m->start_x + m->col * m->start_y)
                    is_path = 1;
            }
        }
    }
    free(visited);
    free_queue(q);
    if (is_path == 0) {
        fprintf(stderr, "Nie znaleziono sciezki\n");
        free(parents);
        return NULL;
    }
    return parents;
}

void add_parent(long long node, long long parent, uint16_t *v, maze m) {
    int mv_x[4] = {0, 1, 0, -1};
    int mv_y[4] = {1, 0, -1, 0};
    int i;
    int node_x = node % m->col;
    int node_y = node / m->col;
    int parent_x = parent % m->col;
    int parent_y = parent / m->col;
    for (i = 0; i < 4; i++) {
        if (node_x + mv_x[i] == parent_x && node_y + mv_y[i] == parent_y) {
            switch (i) {
                case 0:
                    set_bit(0, 2 * node, v);
                    set_bit(0, 2 * node + 1, v);
                    break;
                case 1:
                    set_bit(1, 2 * node, v);
                    set_bit(0, 2 * node + 1, v);
                    break;
                case 2:
                    set_bit(0, 2 * node, v);
                    set_bit(1, 2 * node + 1, v);
                    break;
                case 3:
                    set_bit(1, 2 * node, v);
                    set_bit(1, 2 * node + 1, v);
                    break;
            }
        }
    }
}

long long get_parent(long long node, uint16_t *v, maze m) {
    int a = get_bit(2 * node, v);
    int b = get_bit(2 * node + 1, v);
    if (a == 0 && b == 1)
        return node - m->col;
    if (a == 0 && b == 0)
        return node + m->col;
    if (a == 1 && b == 0)
        return node + 1;
    if (a == 1 && b == 1)
        return node - 1;
    return -1;
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