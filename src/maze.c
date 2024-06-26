#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "maze.h"
#include "txt_management.h"
#include "bin_management.h"

void print_help() { //Funkcja wypisująca help
	printf("Uzycie: ./main.out [FILE] [OPTIONS]\n");
	printf("Program wyszukuje najkrotsza sciezke w labiryncie.\n");
	printf("Plik labiryntu może być w formie tekstowej lub binarnej.\n");
	printf("Wynik dzialania programu wypisywany jest na standardowe wyjscie\n");
	printf("Ponadto program moze zapisac sciezke w pliku binarnym podanym na wejsciu\n");
	printf("lub stworzyc plik binarny i w nim zapisac labirynt oraz sciezke jesli zostal podany plik tekstowy.\n");
	printf("Opcje:\n");
	printf(" -h\twyświetla krotki opis programu, sposob jego uzycia oraz opcjonalne parametry\n");
	printf(" -f\tumozliwia zapis znalezionej sciezki do pliku binarnego. Mozna podac w przypadku pliku binarnego lub po podaniu opcji -b dla pliku tekstowego\n");
	printf(" -b\tumozliwia zapis labiryntu w postaci pliku binarnego w lokalizacji podanej po parametrze. Dziala jedynie w przypadku pliku tekstowego.\n");
	printf(" -t\tumożliwia zapis labiryntu w postaci tekstowej w lokalizacji podanej po parametrze. Dziala jednynie w przypadku pliku binarnego\n");
	printf(" -p\tumożliwia odczyt ścieżki zapisanej w podanym labiryncie binarnym, ignoruje inne opcje\n");
}

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

void set_bit(int value, int n, uint16_t* v) { //Funkcja zapisująca bit do tablicy
	if (value != 0)
		v[n / BIT_TABLE_SIZE] |= 1 << (n % BIT_TABLE_SIZE);
	else
		v[n / BIT_TABLE_SIZE] &= ~(1 << (n % BIT_TABLE_SIZE));
}

int get_bit(int n, uint16_t* v) { //Funkcja zwracająca bit z tablicy
	return ((v[n / BIT_TABLE_SIZE] & (1 << (n % BIT_TABLE_SIZE))) != 0);
}

int is_wall(uint16_t a, uint16_t b, uint16_t x, uint16_t y, maze m) { //Funcja zwracająca informacje czy istnieje połączenie między podanymi węzłami
	int tmp;
	if (a == x && b == y)
		return 3;
	if (a >= m->col || x >= m->col || b >= m->row || y >= m->row)
		return 4;
	if (a == x) {
		if (y < b) {
			tmp = b;
			b = y;
			y = tmp;
		}
		if (y - b != 1)
			return 2;
		tmp = get_bit((m->col * 2 - 1) * b + x + m->col - 1, m->v);
		return tmp;
	}
	else if (b == y) {
		if (x < a) {
			tmp = a;
			a = x;
			x = tmp;
		}
		if (x - a != 1)
			return 2;
		tmp = get_bit((m->col* 2 - 1) * y + a, m->v);
		return tmp;
	}
	else
		return 2;
}

void free_maze(maze m) { //Funcja zwalniająca pamięć struktury labiryntu
	if (m != NULL && m->v != NULL)
		free(m->v);
	if (m != NULL)
		free(m);
}

void fprint_maze(FILE* f, maze m) { //Funkcja wypisująca labirynt
	int i, j, n = 0;
	for (i = 0; i < m->col * 2 + 1; i++) {
		if (m->start_direction == 'N' && i == (m->start_x * 2 + 1))
			fprintf(f, "P");
		else if (m->end_direction == 'N' && i == (m->end_x * 2 + 1))
			fprintf(f, "K");
		else
			fprintf(f, "X");
	}
	if (m->start_direction == 'W' && m->start_y == 0)
		fprintf(f, "\nP ");
	else if (m->end_direction == 'W' && m->end_y == 0)
		fprintf(f, "\nK ");
	else
		fprintf(f, "\nX ");
	for (i = 0; i < m->col - 1; i++) {
		if (get_bit(n, m->v) == 1)
			fprintf(f, "X");
		else
			fprintf(f, " ");
		n++;
		fprintf(f, " ");

	}
	if (m->start_direction == 'E' && m->start_y == 0)
		fprintf(f, "P\n");
	else if (m->end_direction == 'E' && m->end_y == 0)
		fprintf(f, "K\n");
	else
		fprintf(f, "X\n");
	for (i = 0; i < m->row - 1; i++){
		fprintf(f, "X");
		for (j = 0; j < m->col; j++) {
			if(get_bit(n, m->v) == 1)
				fprintf(f, "X");
			else
				fprintf(f, " ");
			n++;
			fprintf(f, "X");
		}
		if (m->start_direction == 'W' && i + 1 == m->start_y)
			fprintf(f, "\nP ");
		else if (m->end_direction == 'W' && i + 1 == m->end_y)
			fprintf(f, "\nK ");
		else
			fprintf(f, "\nX ");
		for (j = 0; j < m->col - 1; j++) {
			if (get_bit(n, m->v) == 1)
				fprintf(f, "X");
			else
				fprintf(f, " ");
			n++;
			fprintf(f, " ");
		}
		if (m->start_direction == 'E' && i + 1 == m->start_y)
			fprintf(f, "P\n");
		else if (m->end_direction == 'E' && i + 1 == m->end_y)
			fprintf(f, "K\n");
		else
			fprintf(f, "X\n");
	}
	for (i = 0; i < m->col * 2 + 1; i++) {
		if (m->start_direction == 'S' && i == (m->start_x * 2 + 1))
			fprintf(f, "P");
		else if (m->end_direction == 'S' && i == (m->end_x * 2 + 1))
			fprintf(f, "K");
		else
			fprintf(f, "X");
	}
	if (f == stdout)
		printf("\n");
}
