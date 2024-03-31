#include <stdio.h>
#include "file_menagement.h"
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Nie podano sciezki do pliku wejsciowego\n");
		return 1;
	}
	maze m = read_file(argv[1]);
	if (m == NULL) {
		fprintf(stderr, "Nie udalo sie zaladowac pliku\n");
		return 2;
	}
	print_maze(m);
	free_maze(m);
	return 0;
}
