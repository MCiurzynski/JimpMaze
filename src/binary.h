#ifndef BINARY_H
#define BINARY_H

#pragma pack(push, 1)
typedef struct bin_buff {
	int n;
	char c;
	FILE* f;
	char sep;
} *buff;

typedef struct header {
    uint32_t name;
	uint8_t esc;
	uint16_t col;
	uint16_t row;
	uint16_t start_x;
	uint16_t start_y;
	uint16_t end_x;
	uint16_t end_y;
    uint32_t reserved[3];
	uint32_t counter;
	uint32_t solution;
	uint8_t sep;
	uint8_t wall;
	uint8_t path;
} header;
#pragma pack(pop)

int convert_to_binary(maze, char*, char*);

int is_bin_file_correct(FILE *);

char get_char(buff);

maze read_bin_maze(FILE *);

#endif
