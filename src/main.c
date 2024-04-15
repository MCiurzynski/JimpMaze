#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "maze.h"
#include "binary.h"
#include "txt_management.h"
#include "find_path.h"

int main(int argc, char **argv) {
	int c, bflag = 0, fflag = 0, tflag = 0, pflag = 0;
	char *bin_file = NULL, *txt_file;
	while ((c = getopt(argc, argv, ":hb:ft:p")) != -1) {
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
			case 't':
				tflag = 1;
				txt_file = optarg;
				break;
			case 'p':
				pflag = 1;
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
	if (fflag == 1 && is_binary(argv[optind]) == 0 && bflag == 0) {
		fprintf(stderr, "Opcja -f wymaga pliku binarnego\n");
		return 6;
	}
	if (pflag == 1 && (fflag == 1 || bflag == 1 || tflag == 1)) {
		fprintf(stderr, "Opcja -p wystepuje bez innych opcji\n");
		return 7;
	}
	if (fflag == 1 && is_binary(argv[optind])) {
		bin_file = argv[optind];
	}
	if (tflag == 1 && is_binary(argv[optind]) == 0) {
		fprintf(stderr, "Opcja -t wymaga pliku binarnego\n");
		return 7;
	}
	if (pflag == 1) {
		read_path_from_bin(argv[optind]);
		return 0;
	}
	maze m = read_file(argv[optind]);
	if (m == NULL) {
		fprintf(stderr, "Nie udalo sie zaladowac pliku\n");
		return 8;
	}
	if (bflag == 1) {
		if (convert_to_binary(m, argv[optind], bin_file) != 0) {
			fprintf(stderr, "Nie udalo sie przekonwertowac na plik binarny\n");
			free_maze(m);
			return 9;
		}
	}
	if (tflag == 1) {
		if (access(txt_file, F_OK) == 0) {
			fprintf(stderr, "Plik o nazwie %s istnieje\n", txt_file);
			free_maze(m);
			return 10;
		}
		else {
			FILE* f = fopen(txt_file, "w");
			if (f == NULL) {
				fprintf(stderr, "Nie udalo sie stworzyc pliku tekstowego\n");
				free_maze(m);
			return 11;
			}
			else {
				fprint_maze(f, m);
				fclose(f);
			}
		}
	}
	if (find_path(m) != 0) {
		fprintf(stderr, "Nie udalo sie znalezc siezki\n");
		free_maze(m);
		return 112;
	}
	if (fflag == 1) {
		find_path_to_bin(m, bin_file);
	}
	free_maze(m);
	return 0;
}
