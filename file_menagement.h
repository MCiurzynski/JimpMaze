#ifndef FILE_MENAGEMENT
#define FILE_MENAGEMENT

typedef struct maze_s {
	FILE* f;
	int n;
	int m;
} *maze;

int is_file_correct(maze);

char get_char(maze, int, int);

#endif
