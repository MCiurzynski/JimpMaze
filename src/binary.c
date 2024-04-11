#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include "maze.h"
#include "binary.h"

int convert_to_binary(maze m, char* source, char* bin) {//Funkcja in progress
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
	header h;
	h.name = 0x52524243;
	h.esc = 0x1b;
	h.col = m->col * 2 + 1;
	h.row = m->row * 2 + 1;
	fwrite(&h, sizeof h, 1, b);
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
