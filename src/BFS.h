#include <stdbool.h>
#ifndef BFS_H
#define BFS_H

typedef struct {
	int *queue;
	int front;
	int back;
	int length;
	bool isEmpty;
} *queue_tt; 

queue_tt queueInit( int );

bool queueAdd( queue_tt, int );

int queuePop( queue_tt );

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
