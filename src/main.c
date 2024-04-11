#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "maze.h"
#include "binary.h"

int main(int argc, char **argv) {
	int c, bflag = 0, fflag = 0;
	char *bin_file;
	while ((c = getopt(argc, argv, ":hb:f")) != -1) {
		switch (c) {
			case 'h':
				print_help();
				return 0;
			case 'b':
				bflag = 1;
				bin_file = optarg;
				break;
			case 'f':
				fflag = 1;
				break;
			case '?':
				fprintf(stderr, "Nieznana opcja: -%c\n", optopt);
				return 1;
			case ':':
				fprintf(stderr, "Argument dla opcji -%c jest wymagany\n", optopt);
				return 2;
		}
	}
	if (argc - optind == 0) {
		fprintf(stderr, "Nie podano sciezki do pliku wejsciowego\n");
		return 3;
	}
	if (argc - optind > 1) {
		fprintf(stderr, "Podano za duzo plikow wejsciowych\n");
		return 4;
	}
	if (bflag == 1 && is_txt(argv[optind]) == 0) {
		fprintf(stderr, "Opcja -b wymaga pliku tekstowego\n");
		return 5;
	}
	if (fflag == 1 && is_binary(argv[optind]) == 0) {
		fprintf(stderr, "Opcja -f wymaga pliku binarnego\n");
		return 6;
	}
	maze m = read_file(argv[optind]);
	if (m == NULL) {
		fprintf(stderr, "Nie udalo sie zaladowac pliku\n");
		return 7;
	}
	if (bflag == 1) {
		printf("%d\n", convert_to_binary(m, argv[optind], bin_file));
		printf("weszlo\n");
	}
	//print_maze(m);
	free_maze(m);
	return 0;
}
