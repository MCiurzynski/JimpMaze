#include <stdio.h>
#include "file_menagement.h"

#include <stdlib.h>

int main(int argc, char **argv) {
	if (argc == 1) {
		fprintf(stderr, "ERROR: Nie podano nazwy pliku\n");
		return 1;
	}
	FILE* f = fopen(argv[1], "r");
	if (f == NULL) {
		fprintf(stderr, "ERROR: Nie udalo se otworzyc pliku\n");
		return 2;
	}
	maze m = malloc(sizeof(&m));
	m->m = 1;
	m->f = f;
	if (is_file_correct(m) == 0) {
		fprintf(stderr, "ERROR: Podany plik nie jest poprawny\n");
		return 3;
	}
	printf("Plik jest prawidlowy\n");
	int i, j;
	for (i = 0; i < m->m; i++) {
		for(j = 0; j < m->n; j++)
			printf("%c", get_char(m, i, j));
		printf("\n");
	}
	free(m);
	fclose(f);
	return 0;
}
