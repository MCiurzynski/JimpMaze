#include <stdbool.h>
#ifndef BFS_H
#define BFS_H

FILE * charFileInit( int, char* );

unsigned char* intToChars( int );

int charsToInt( unsigned char* );

void parentsAdd( FILE *, int, int );

int intParentsGet( FILE *, int );

int BFS( maze );

int pathLength( FILE *, int, int );

char *pathToFile( FILE *, int, int, int );

void pathConvert( char *, maze, int );

#endif