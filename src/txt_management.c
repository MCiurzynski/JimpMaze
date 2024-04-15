#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "maze.h"
#include "txt_management.h"

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