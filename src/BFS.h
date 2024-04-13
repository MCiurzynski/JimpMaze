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

FILE * parentsInit( int );

unsigned char* intToChars( int );

int charsToInt( char* );

void parentsAdd( FILE *, int, int );

int parentsGet( FILE *, int );

int BFS( maze );


#endif
