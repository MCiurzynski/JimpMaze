#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "file_menagement.h"

#define MAX_SIZE 2050

maze read_file(char* name) { // Główna funkcja wczytująca pliki
	maze m;
	FILE* f;
	if (is_txt(name) == 1) { //Sprawdzanie typu pliku
		f = fopen(name, "r");
		if (f == NULL) {
			fprintf(stderr, "Nie udalo sie otworzyc pliku\n");
			return NULL;
		}
		if (is_file_correct(f) == 1) { //Sprawdzanie poprawności pliku
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
		f = fopen(name, "rb");
		if (f == NULL) {
			fprintf(stderr, "Nie udalo sie otworzyc pliku\n");
			return NULL;
		}
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
		return NULL;
	}
}

int is_txt(char *name) { //Funkcja sprawdzająca czy podany plik jest jest .txt
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

int is_binary(char *name) { //Funkcja sprawdzająca czy podany plik jest .bin
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


int is_file_correct(FILE* f) { //Funkcja sprawdzająca czy podany plik tekstowy jest poprawny
	char c;
	char buff[MAX_SIZE];
	int p = 0, k = 0, col, row, len, i;
	while ((c = fgetc(f)) != EOF) { //Sprawdzanie ilości wejść i wyjść
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
	fgets(buff, MAX_SIZE, f); //Sprawdzanie ilości kolumn
	col = strlen(buff);
	fseek(f, 0, SEEK_SET);
	if (buff[col - 1] != '\n') //Sprawdzenie czy kolumna nie jest większa niż MAX_SIZE
		return 0;
	row = 0;
	while (fgets(buff, MAX_SIZE, f) != NULL) { //Sprawdzanie ilości wierszy
		row++;
		len = strlen(buff);
		if (len != col) //Sprawdzanie czy każdy wiersz ma taką samą długość
			return 0;
		if (buff[0] != 'X' && buff[0] != 'P' && buff[0] != 'K') //Sprawdzanie czy wiersz zaczyna i kończy się ścianą
			return 0;
		if (buff[len - 2] != 'X' && buff[len - 2] != 'P' && buff[len - 2] != 'K')
			return 0;
		if (buff[col - 1] != '\n') //Sprawdzanie czy wiersz nie jest większy niż MAX_SIZE
			return 0;
		if (row % 2 == 0) {
			for (i = 1; i < col - 1; i += 2)
				if (buff[i] != ' ')
					return 0;
		}
		else {
			for (i = 2; i < col; i += 2)
				if (buff[i] != 'X')
					return 0;
		}
	}
	if (col < 4 || row < 3) //Sprawdzanie czy labirynt ma minimanlą wielkość
		return 0;
	return 1;
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

maze read_maze(FILE *f) { //Funcja wczytująca plik tekstowy
	char c;
	int i = 0, n = 0, j, row = 0;
	fseek(f, 0, SEEK_SET);
	maze m = malloc(sizeof(*m));
	if (m == NULL) {
		fprintf(stderr, "Nie udalo sie zaalokowac pamieci\n");
		return NULL;
	}
	m->col = 0;
	while ((c = fgetc(f)) != '\n') //Liczenie ilości kolumn
		m->col++;
	m->row = 1;
	while((c = fgetc(f)) != EOF) //Liczenie ilości wierszy
		if (c == '\n')
			m->row++;
	m->col = (m->col - 1) / 2;
	m->row = (m->row - 1) / 2;
	fseek(f, 0, SEEK_SET);
	while ((c = fgetc(f)) != EOF) { //Zapisywanie współrzędnych i ścian wejścia i wyjścia
		if (c == 'P') {
			m->start_y = row;
			m->start_x = i % (m->col * 2 + 1);
			if (m->start_x == 0) {
				m->start_y = (m->start_y - 1) / 2;
				m->start_direction = 'W';
			}
			else if (m->start_x == m->col * 2) {
				m->start_x = (m->start_x - 1) / 2;
				m->start_y = (m->start_y - 1) / 2;
				m->start_direction = 'E';
			}
			else if (m->start_y == 0) {
				m->start_x = (m->start_x - 1) / 2;
				m->start_direction = 'N';
			}
			else if (m->start_y == m->row * 2) {
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
			else if (m->end_x == m->col * 2) {
				m->end_x = (m->end_x - 1) / 2;
				m->end_y = (m->end_y - 1) / 2;
				m->end_direction = 'E';
			}
			else if (m->end_y == 0) {
				m->end_x = (m->end_x - 1) / 2;
				m->end_direction = 'N';
			}
			else if (m->end_y == m->row * 2) {
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
	m->v = calloc(((m->col - 1) * m->row + (m->row - 1) * m->col) / 16 + 1, sizeof(uint16_t));
	if (m->v == NULL) {
		fprintf(stderr, "Nie udalo sie zaalokowac pamieci na tablice\n");
		free(m);
		return NULL;
	}
	fseek(f, (2 * m->col) + 3, SEEK_SET);
	for (i = 0; i < m->col - 1; i++) { //Wczytywanie pierwszego wiersza połączeń
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
	for (i = 0; i < m->row - 1; i++) { //Wczytywanie pozostałych połączeń
		fseek(f, 3, SEEK_CUR);
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
		fseek(f, 3, SEEK_CUR);
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
