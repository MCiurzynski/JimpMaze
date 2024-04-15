#ifndef BFS_FOR_BIG_H
#define BFS_FOR_BIG_H

FILE * charFileInit( int, char* );

unsigned char* intToChars( int );

int charsToInt( unsigned char* );

void parentsAdd( FILE *, int, int );

int intParentsGet( FILE *, int );

int bfs_for_big( maze, int b, char* );

int pathLength( FILE *, int, int );

char *pathToFile( FILE *, int, int, int );

void pathConvert( char *, maze, int );

int pathConvertToBin( char *, maze, int, char *);

#endif
