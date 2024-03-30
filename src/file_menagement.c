#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "file_menagement.h"

#define MAX_SIZE 2050

maze read_file(char* name) {
	maze m;
	FILE* f = fopen(name, "r");
	if (f == NULL) {
		fprintf(stderr, "Nie udalo sie otworzyc pliku\n");
		return NULL;
	}
	if (is_txt(name) == 1) {
		if (is_file_correct(f) == 1) {
			m = read_maze(f);
			fclose(f);
			return m;
		}
		else {
			fprintf(stderr, "Plik jest nieprawidlowy\n");
			fclose(f);
			return NULL;
		}
	}
	else if (is_binary(name) == 1) {
		if (is_bin_file_correct(f) == 1) {
			m = read_bin_maze(f);
			fclose(f);
			return m;
		}
		else {
			fprintf(stderr, "Plik jest nieprawidlowy\n");
			fclose(f);
			return NULL;
		}
	}
	else {
		fprintf(stderr, "Nieprawidlowe rozszerzenie pliku\n");
		fclose(f);
		return NULL;
	}
}

int is_txt(char *name) {
	int i, len;
	char tmp[5];
	len = strlen(name);
	if (len < 4)
		return 0;
	for (i = len - 4; i < len; i++)
		tmp[i - (len - 4)] = name[i];
	tmp[4] = '\0';
	if (strcmp(tmp, ".txt") == 0)
		return 1;
	else
		return 0;
}

int is_binary(char *name) {
	int i, len;
	char tmp[5];
	len = strlen(name);
	if (len < 4)
		return 0;
	for (i = len - 4; i < len; i++)
		tmp[i - (len - 4)] = name[i];
	tmp[4] = '\0';
	if (strcmp(tmp, ".bin") == 0)
		return 1;
	else
		return 0;
}


int is_file_correct(FILE* f) {
	char c;
	char buff[MAX_SIZE];
	int p = 0, k = 0, col = 0, row = 0;
	while ((c = fgetc(f)) != EOF) {
		if (c != 'X' && c != ' ' && c != '\n' && c != 'P' && c != 'K')
			return 0;
		if (c == 'P')
			p++;
		if (c == 'K')
			k++;
	}
	fseek(f, 0, SEEK_SET);
	if (k != 1 || p != 1)
		return 0;
	fgets(buff, MAX_SIZE, f);
	col = strlen(buff);
	fseek(f, 0, SEEK_SET);
	if (buff[col - 1] != '\n')
		return 0;
	while (fgets(buff, MAX_SIZE, f) != NULL) {
		row++;
		if (strlen(buff) != col)
			return 0;
		if (buff[col - 1] != '\n')
			return 0;
	}
	if (col < 4 || row < 3)
		return 0;
	return 1;
}

int is_bin_file_correct(FILE *f) {
	uint8_t esc;
	uint16_t col;
	uint16_t row;
	uint16_t start_x;
	uint16_t start_y;
	uint16_t end_x;
	uint16_t end_y;
	uint32_t counter;
	uint32_t solution;
	uint8_t sep;
	uint8_t wall;
	uint8_t path;
	uint8_t c;
	int slowa_klucz = 0, tmp = 0, symbols = 0;
	fseek(f, 4, SEEK_CUR);
	fread(&esc, 1, 1, f);
	fread(&col, 2, 1, f);
	fread(&row, 2, 1, f);
	fread(&start_x, 2, 1, f);
	fread(&start_y, 2, 1, f);
	fread(&end_x, 2, 1, f);
	fread(&end_y, 2, 1, f);
	fseek(f, 12, SEEK_CUR);
	fread(&counter, 4, 1, f);
	fread(&solution, 4, 1, f);
	fread(&sep, 1, 1, f);
	fread(&wall, 1, 1, f);
	if (fread(&path, 1, 1, f) == 0)
		return 0;
	printf("Col: %d\nRow: %d\nStart x: %d\nStart y: %d\nEnd_x: %d\nEnd_y: %d\nCounter: %d\nSolution: %d\nSep: %c\nWall: %c\nPath: %c\n", col, row, start_x, start_y, end_x, end_y, counter, solution, sep, wall, path);
	while(fread(&c, 1, 1, f) == 1) {
		if (c == sep) {
			tmp = 0;
			slowa_klucz++;
		}
		if (tmp == 2)
			symbols += c + 1;
		tmp++;
	}
	if (slowa_klucz != counter)
		return 0;
	if (symbols != col * row)
		return 0;
	return 1;
}

maze read_maze(FILE *f) {
	char c;
	int i = 0, n = 0, j, row = 0;
	fseek(f, 0, SEEK_SET);
	maze m = malloc(sizeof(*m));
	if (m == NULL) {
		fprintf(stderr, "Nie udalo sie zaalokowac pamieci\n");
		return NULL;
	}
	m->col = 0;
	while ((c = fgetc(f)) != '\n')
		m->col++;
	m->row = 1;
	while((c = fgetc(f)) != EOF)
		if (c == '\n')
			m->row++;
	m->col = (m->col - 1) / 2;
	m->row = (m->row - 1) / 2;
	fseek(f, 0, SEEK_SET);
	while ((c = fgetc(f)) != EOF) {
	if (c == 'P') {
		m->start_y = row;
		m->start_x = i % (m->col * 2 + 1);
		if (m->start_x == 0) {
			m->start_y = (m->start_y - 1) / 2;
			m->start_direction = 'W';
		}
		if (m->start_x == m->col * 2) {
			m->start_x = (m->start_x - 1) / 2;
			m->start_y = (m->start_y - 1) / 2;
			m->start_direction = 'E';
		}
		if (m->start_y == 0) {
			m->start_x = (m->start_x - 1) / 2;
			m->start_direction = 'N';
		}
		if (m->start_y == m->row * 2) {
			m->start_x = (m->start_x - 1) / 2;
			m->start_y = (m->start_y - 1) / 2;
			m->start_direction = 'S';
		}
	}
	if (c == 'K') {
		m->end_y = row;
		m->end_x = i % (m->col * 2 + 1);
		if (m->end_x == 0) {
			m->end_y = (m->end_y - 1) / 2;
			m->end_direction = 'W';
		}
		if (m->end_x == m->col * 2) {
			m->end_x = (m->end_x - 1) / 2;
			m->end_y = (m->end_y - 1) / 2;
			m->end_direction = 'E';
		}
		if (m->end_y == 0) {
			m->end_x = (m->end_x - 1) / 2;
			m->end_direction = 'N';
		}
		if (m->end_y == m->row * 2) {
			m->end_x = (m->end_x - 1) / 2;
			m->end_y = (m->end_y - 1) / 2;
			m->end_direction = 'S';
		}
	}
		if (c != '\n')
			i++;
		if (c == '\n')
			row++;
	}
	printf("Col: %d\nRow: %d\nStart x: %d\nStart y: %d\nDirection: %c\nEnd x: %d\nEnd y: %d\nDirection: %c\n", m->col, m->row, m->start_x, m->start_y, m->start_direction, m->end_x, m->end_y, m->end_direction);
	fseek(f, 0, SEEK_SET);
	c = 'X';
	while (c != '\n' && c != EOF)
		c = fgetc(f);
	fgetc(f);
	m->v = calloc(((m->col - 1) * m->row + (m->row - 1) * m->col) / 16 + 1, sizeof(uint16_t));
	if (m->v == NULL) {
		fprintf(stderr, "Nie udalo sie zaalokowac pamieci na tablice\n");
		free(m);
		return NULL;
	}
	for (i = 0; i < m->col - 1; i++) {
		fgetc(f);
		if (fgetc(f) == 'X') {
			set_bit(1, n, m->v);
			n++;
		}
		else {
			set_bit(0, n, m->v);
			n++;
		}
	}
	for (i = 0; i < m->row - 1; i++) {
		while(fgetc(f) != '\n');
		for (j = 0; j < m->col; j++) {
			fgetc(f);
			if (fgetc(f) == 'X') {
				set_bit(1, n, m->v);
				n++;
			}
			else {
				set_bit(0, n, m->v);
				n++;
			}
		}
		while(fgetc(f) != '\n');
		fgetc(f);
		for (j = 0; j < m->col - 1; j++) {
			fgetc(f);
			if (fgetc(f) == 'X') {
				set_bit(1, n, m->v);
				n++;
			}
			else {
				set_bit(0, n, m->v);
				n++;
			}
		}
	}
	return m;
}

maze read_bin_maze(FILE *f) {
	fseek(f, 0, SEEK_SET);
	uint8_t esc;
	uint16_t col;
	uint16_t row;
	uint16_t start_x;
	uint16_t start_y;
	uint16_t end_x;
	uint16_t end_y;
	uint32_t counter;
	uint32_t solution;
	uint8_t sep;
	uint8_t wall;
	uint8_t path;
	uint8_t c;
	char buff[257];
	fseek(f, 4, SEEK_CUR);
	fread(&esc, 1, 1, f);
	fread(&col, 2, 1, f);
	fread(&row, 2, 1, f);
	fread(&start_x, 2, 1, f);
	fread(&start_y, 2, 1, f);
	fread(&end_x, 2, 1, f);
	fread(&end_y, 2, 1, f);
	fseek(f, 12, SEEK_CUR);
	fread(&counter, 4, 1, f);
	fread(&solution, 4, 1, f);
	fread(&sep, 1, 1, f);
	fread(&wall, 1, 1, f);
	fread(&path, 1, 1, f);
	maze m = malloc(sizeof(*m));
	if (m == NULL) {
		fprintf(stderr, "Nie udalo sie zaalokowac pamieci\n");
		return NULL;
	}
	start_x--;
	start_y--;
	end_x--;
	end_y--;
	m->col = (col - 1) / 2;
	m->row = (row - 1) / 2;
	if (start_x == 0) {
		m->start_y = (start_y - 1) / 2;
		m->start_direction = 'W';
	}
	if (start_x == m->col * 2) {
		m->start_x = (start_x - 1) / 2;
		m->start_y = (start_y - 1) / 2;
		m->start_direction = 'E';
	}
	if (start_y == 0) {
		m->start_x = (start_x - 1) / 2;
		m->start_direction = 'N';
	}
	if (start_y == m->row * 2) {
		m->start_x = (start_x - 1) / 2;
		m->start_y = (start_y - 1) / 2;
		m->start_direction = 'S';
	}
	if (end_x == 0) {
		m->end_y = (end_y - 1) / 2;
		m->end_direction = 'W';
	}
	if (end_x == m->col * 2) {
		m->end_x = (end_x - 1) / 2;
		m->end_y = (end_y - 1) / 2;
		m->end_direction = 'E';
	}
	if (end_y == 0) {
		m->end_x = (end_x - 1) / 2;
		m->end_direction = 'N';
	}
	if (end_y == m->row * 2) {
		m->end_x = (end_x - 1) / 2;
		m->end_y = (end_y - 1) / 2;
		m->end_direction = 'S';
	}
	m->v = calloc(((m->col - 1) * m->row + (m->row - 1) * m->col) / 16 + 1, sizeof(uint16_t));
	if (m->v == NULL) {
		fprintf(stderr, "Nie udalo sie zaalokowac pamieci na tablice\n");
		free(m);
		return NULL;
	}
	printf("%d %d %d %d %d %d\n", m->col, m->row, start_x, start_y, end_x, end_y);
	printf("%d %d %d %d %d %d\n", m->col, m->row, m->start_x, m->start_y, m->end_x, m->end_y);
	return m;
}
