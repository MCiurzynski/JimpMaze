#ifndef TXT_MANAGEMENT
#define TXT_MANAGEMENT

#define MAX_SIZE 2050

int is_file_correct(FILE *);

maze read_maze(FILE *);

void fprint_maze(FILE*, maze);

#endif
