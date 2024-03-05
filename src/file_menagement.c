#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "file_menagement.h"

int is_file_correct(maze m) {
	char c;
	char buff[2049];
	int p = 0, k = 0;
	while ((c = fgetc(m->f)) != EOF) {
		if (c != 'X' && c != ' ' && c != '\n' && c != 'P' && c != 'K')
			return 0;
		if (c == 'P')
			p++;
		if (c == 'K')
			k++;
	}
	if (k != 1 || p != 1)
		return 0;
	fseek(m->f, 0, SEEK_SET);
	fgets(buff, 2049, m->f);
	m->n = strlen(buff);
	if (buff[m->n-1] != '\n')
		return 0;
	while (fgets(buff, 2049, m->f) != NULL) {
		m->m++;
		if (strlen(buff) != m->n)
			return 0;
		if (buff[m->n-1] != '\n')
			return 0;
	}
	m->n--;
	if (m->n < 3 || m->m < 3)
		return 0;
	return 1;
}

char get_char(maze m, int a, int b) {
	fseek(m->f, a * m->n + b + a, SEEK_SET);
	return fgetc(m->f);
}
