#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include "maze.h"
#include "binary.h"

int convert_to_binary(maze m, char* source, char* bin) {//Funkcja in progress
	int i, j, n;
	char c, symbol;
	uint32_t counter;
	header h;
	if (access(bin, F_OK) == 0) {
		fprintf(stderr, "Plik o nazwie \"%s\" istnieje\n", bin);
		return 1;
	}
	FILE* b = fopen(bin, "wb");
	if (b == NULL) {
		fprintf(stderr, "Nie udalo sie stworzyc pliku o nazwie \"%s\"\n", bin);
		return 2;
	}
	FILE* s = fopen(source, "r");
	if (s == NULL) {
		fclose(b);
		return 3;
	}
	h.name = 0x52524243;
	h.esc = 0x1b;
	h.col = m->col * 2 + 1;
	h.row = m->row * 2 + 1;
	for (i = 1; i <= m->row * 2 + 1; i++) {
		for (j = 1; j <= m->col * 2 + 1; j++) {
			c = fgetc(s);
			if (c == 'P') {
				h.start_x = j;
				h.start_y = i;
			}
			if (c == 'K') {
				h.end_x = j;
				h.end_y = i;
			}
		}
		fgetc(s);
	}
	fseek(s, 0, SEEK_SET);
	h.reserved[0] = 0xFF;
	h.reserved[1] = 0xFF;
	h.reserved[2] = 0xFF;
	h.counter = 0;
	h.solution = 0;
	h.sep = '#';
	h.wall = 'X';
	h.path = ' ';
	fwrite(&h, sizeof h, 1, b);
	counter = 0;
	n = -1;
	symbol = 0;
	for (i = 0; i < m->row * 2 + 1; i++) {
		for (j = 0; j < m->col * 2 + 1; j++) {
			c = fgetc(s);
			if (symbol == c || (symbol == ' ' && (c == 'P' || c == 'K'))) {
				n++;
				if (n == 255) {
					fwrite(&h.sep, 1, 1, b);
					fwrite(&symbol, 1, 1, b);
					fwrite(&n, 1, 1, b);
					counter++;
					n = -1;
					symbol = 0;
				}
			}
			else {
				if (symbol != 0) { 
					fwrite(&h.sep, 1, 1, b);
					fwrite(&symbol, 1, 1, b);
					fwrite(&n, 1, 1, b);
					counter++;
				}
				n = 0;
				if (c == 'P' || c == 'K')
					c = ' ';
				symbol = c;
			}
		}
		if (symbol != 0) {
			fwrite(&h.sep, 1, 1, b);
			fwrite(&symbol, 1, 1, b);
			fwrite(&n, 1, 1, b);
			counter++;
			n = -1;
			symbol = 0;
		}
		fgetc(s);
	}
	fwrite(&h.name, sizeof h.name, 1, b);
	fseek(b, 29, SEEK_SET);
	fwrite(&counter, sizeof counter, 1, b);
	fclose(b);
	fclose(s);
	return 0;
}

int is_bin_file_correct(FILE *f) { //Funkcja sprawdzająca czy podany plik binarny jest prawidłowy
	header h;
	uint8_t c;
	int sepcount = 0, tmp = 0, symbols = 0;
	if (fread(&h, sizeof h, 1, f) != 1) //Sprawdzanie czy plik się nie skończył
		return 0;
	while(fread(&c, 1, 1, f) == 1) { //Liczenie słów kluczowych i symboli
		if (c == h.sep) {
			tmp = 0;
			sepcount++;
		}
		if (tmp == 2)
			symbols += c + 1;
		tmp++;
	}
	if (sepcount != h.counter) //Sprawdzanie zgodności ilości słów kluczowych i symboli z wartościami podanymi w pliku
		return 0;
	if (symbols != h.col * h.row)
		return 0;
	return 1;
}

char get_char(buff bin) { //Funkcja pomocnicza zwracająca znaki labiryntu z pliku binarnego 
	uint8_t c;
	int x;
	if (bin->n <= 0) {
		do {
			x = fread(&c, 1, 1, bin->f); //Szukanie separatora
		}
		while (c != bin->sep && x != 1);
		if (x == 0)
			return 0;
		fread(&(bin->c), 1, 1, bin->f);
		fread(&(bin->n), 1, 1, bin->f);
		bin->n++;
	}
	bin->n--;
	return bin->c;
}

maze read_bin_maze(FILE *f) { //Funkcja wczytująca dane z pliku binarnego
	fseek(f, 0, SEEK_SET);
	header h;
	fread(&h, sizeof h, 1, f);
	maze m = malloc(sizeof(*m));
	if (m == NULL) {
		fprintf(stderr, "Nie udalo sie zaalokowac pamieci\n");
		return NULL;
	}
	h.start_x--;
	h.start_y--;
	h.end_x--;
	h.end_y--;
	m->col = (h.col - 1) / 2;
	m->row = (h.row - 1) / 2;
	if (h.start_x == 0) { //Zapisywanie współrzednych i ścian labiryntu
		m->start_y = (h.start_y - 1) / 2;
		m->start_direction = 'W';
	}
	else if (h.start_x == m->col * 2) {
		m->start_x = (h.start_x - 1) / 2;
		m->start_y = (h.start_y - 1) / 2;
		m->start_direction = 'E';
	}
	else if (h.start_y == 0) {
		m->start_x = (h.start_x - 1) / 2;
		m->start_direction = 'N';
	}
	else if (h.start_y == m->row * 2) {
		m->start_x = (h.start_x - 1) / 2;
		m->start_y = (h.start_y - 1) / 2;
		m->start_direction = 'S';
	}
	if (h.end_x == 0) {
		m->end_y = (h.end_y - 1) / 2;
		m->end_direction = 'W';
	}
	else if (h.end_x == m->col * 2) {
		m->end_x = (h.end_x - 1) / 2;
		m->end_y = (h.end_y - 1) / 2;
		m->end_direction = 'E';
	}
	else if (h.end_y == 0) {
		m->end_x = (h.end_x - 1) / 2;
		m->end_direction = 'N';
	}
	else if (h.end_y == m->row * 2) {
		m->end_x = (h.end_x - 1) / 2;
		m->end_y = (h.end_y - 1) / 2;
		m->end_direction = 'S';
	}
	m->v = calloc(((m->col - 1) * m->row + (m->row - 1) * m->col) / 16 + 1, sizeof(uint16_t));
	if (m->v == NULL) {
		fprintf(stderr, "Nie udalo sie zaalokowac pamieci na tablice\n");
		free(m);
		return NULL;
	}
	int i, j, n = 0;
	struct bin_buff bin = {0, 0, f, h.sep};
	for (i = 0; i < m->col * 2 + 2; i++) { //Odrzucanie zbędnych danych
		get_char(&bin);
	}
	for (i = 0; i < m->col - 1; i++) { //Wczytywanie pierwszego wiersza przejść
		get_char(&bin);
		if (get_char(&bin) == h.wall) {
			set_bit(1, n, m->v);
			n++;
		}
		else {
			set_bit(0, n, m->v);
			n++;
		}
	}
	for (i = 0; i < m->row - 1; i++) { //Wczytywanie pozostałych przejść
		for (j = 0; j < 2; j++)
			get_char(&bin);
		for (j = 0; j < m->col; j++) {
			get_char(&bin);
			if (get_char(&bin) == h.wall) {
				set_bit(1, n, m->v);
				n++;
			}
			else {
				set_bit(0, n, m->v);
				n++;
			}
		}
		for (j = 0; j < 2; j++)
			get_char(&bin);
		for (j = 0; j < m->col - 1; j++) {
			get_char(&bin);
			if (get_char(&bin) == h.wall) {
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

int read_path_from_bin(char *bin_file) {
	uint32_t offset, steps;
	uint8_t direction, len, tmp_direction;
	int i;
	FILE *f = fopen(bin_file, "rb");
	if (f == NULL) {
		fprintf(stderr, "Nie udalo sie otworzyc pliku binarnego\n");
		return -1;
	}
	if (is_bin_file_correct(f) == 0) {
		fprintf(stderr, "Wprowadzony plik binarny jest niepoprawny\n");
		fclose(f);
		return -2;
	}
	maze m = read_file(bin_file);
	if (m == NULL) {
		fprintf(stderr, "Nie udalo sie otworzyc labiryntu\n");
		fclose(f);
		return -3;
	}
	fseek(f, 33, SEEK_SET);
	fread(&offset, sizeof offset, 1, f);
	fseek(f, offset + 4, SEEK_SET);
	fread(&steps, sizeof steps, 1, f);
	fread(&direction, sizeof direction, 1, f);
	printf("START\n");
	fread(&len, sizeof len, 1, f);
	tmp_direction = direction;
	if (direction_to_int(tmp_direction) - direction_to_int(m->start_direction) == 1 || direction_to_int(tmp_direction) - direction_to_int(m->start_direction) == -3)
		printf("TURNLEFT\n");
	if (direction_to_int(tmp_direction) - direction_to_int(m->start_direction) == -1 || direction_to_int(tmp_direction) - direction_to_int(m->start_direction) == 3)
		printf("TURNRIGHT\n");
	printf("FORWARD %d\n", len + 1);
	for (i = 0; i < steps + 1; i++) {
		fread(&direction, sizeof len, 1, f);
		fread(&len, sizeof len, 1, f);
		print_direction(tmp_direction, direction);
		printf("FORWARD %d\n", len + 1);
		tmp_direction = direction;
	}
	if (direction_to_int(tmp_direction) - direction_to_int(m->end_direction) == 1 || direction_to_int(tmp_direction) - direction_to_int(m->end_direction) == -3)
		printf("TURNLEFT\n");
	if (direction_to_int(tmp_direction) - direction_to_int(m->end_direction) == -1 || direction_to_int(tmp_direction) - direction_to_int(m->end_direction) == 3)
		printf("TURNRIGHT\n");
	printf("STOP\n");
	free_maze(m);
	return 0;
}

void print_direction(uint8_t x, uint8_t y) {
	int a, b;
	switch (x)
	{
	case 'N':
		a = 0;
		break;
	case 'E':
		a = 1;
		break;
	case 'S':
		a = 2;
		break;
	case 'W':
		a = 3;
		break;
	}
	switch (y)
	{
	case 'N':
		b = 0;
		break;
	case 'E':
		b = 1;
		break;
	case 'S':
		b = 2;
		break;
	case 'W':
		b = 3;
		break;
	}
	if (a - b == 1 || a - b == -3)
		printf("TURNLEFT\n");
	if (a - b == -1 || a - b == 3)
		printf("TURNRIGHT\n");
}

int direction_to_int(uint16_t x) {
	switch (x)
	{
	case 'N':
		return 0;
	case 'E':
		return 1;
	case 'S':
		return 2;
	case 'W':
		return 3;
	default:
		return -1;;
	}
}