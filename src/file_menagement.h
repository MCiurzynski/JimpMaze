#include "maze.h"
#ifndef FILE_MENAGEMENT
#define FILE_MENAGEMENT

typedef struct bin_buff {
	int n;
	char c;
} *buff;

maze read_file(char*);

int is_txt(char*);

int is_binary(char*);

int is_file_correct(FILE *);

int is_bin_file_correct(FILE *);

maze read_maze(FILE *);

char get_char(buff, FILE*, uint8_t);

maze read_bin_maze(FILE *);

#endif
