#include <stdbool.h>
#ifndef BFS_H
#define BFS_H

typedef struct {
	int *queue;
	int front;
	int back;
	int length;
	bool isEmpty;
} *queue_t;

queue_t queueInit( int );

bool queueAdd( queue_t, int );

int queuePop( queue_t );

FILE * charFileInit( int, char* );

//FILE * parentsInit( int );

unsigned char* intToChars( int );

int charsToInt( unsigned char* );

void parentsAdd( FILE *, int, int );

int intParentsGet( FILE *, int );

int BFS( maze );

int pathLength( FILE *, int, int );

char *pathToFile( FILE *, int, int, int );

void fileTest( FILE *, int );

void pathConvert( char *, maze, int );


#endif
