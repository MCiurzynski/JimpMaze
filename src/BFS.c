#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "maze.h"
#include "txt_management.h"
#include "BFS.h"

queue_t queueInit( int size ){

	queue_t queue = malloc( sizeof( *queue ) );
	if( queue->queue == NULL ){
                fprintf( stderr, "Nie udalo sie zaalokowac pamieci na kolejke!" );
                return NULL;
        }

	queue->queue = malloc( size * sizeof(int) );
	if( queue->queue == NULL ){
		fprintf( stderr, "Nie udalo sie zaalokowac pamieci na tablice kolejki!" );
		return NULL;
	}

	queue->front = 0;
	queue->back = 0;
	queue->length = size;
	queue->isEmpty = true;

	return queue;

}

bool queueAdd(queue_t queue, int node) {

	if( queue->isEmpty ){
		queue->queue[queue->back] = node;
		queue->isEmpty = false;
		//printf("Back1 = %d\n", queue->back);
		//printf("Back2 = %d\n", queue->back);
		return true;
		
	} else{
	    	if ((queue->back + 1) % queue->length == queue->front) {
			fprintf(stderr, "Kolejce zabraklo miejsca!!!\n");
			return false;
	    	}
	    	
		//printf("Back1 = %d\n", queue->back);
		queue->back = (queue->back + 1) % ( queue->length );
		queue->queue[queue->back] = node;	
		//printf("Back2 = %d\n", queue->back);
		return true;
	}
}

int queuePop(queue_t queue) {

    	if (queue->front == queue->back) {
		if( queue->isEmpty ){
			fprintf(stderr, "Kolejka jest pusta!\n");
			return -1;
			
		} else{
			int popped = queue->queue[queue->front];
			queue->isEmpty = true;
			return popped;
			
		}
    	}

	int popped = queue->queue[queue->front];
	queue->front = (queue->front + 1) % queue->length;
	return popped;
}

FILE * parentsInit( int size ){

	FILE *parents = fopen( "parents.txt", "w+" );
	if( parents == NULL ){
		fprintf( stderr, "Nie udalo sie utworzyc pliku!" );
                return NULL;
                
	} else{
		for( int i = 0; i < size; i++ ){
			fprintf( parents, "%c%c%c", 255, 255, 255 );
		}
		return parents;
		
	}
}

unsigned char* intToChars(int number) {
	unsigned char *charArray = malloc( 3 * sizeof( char ) );
	charArray[0] = (number >> 16) & 0xFF;
	charArray[1] = (number >> 8) & 0xFF;
	charArray[2] = number & 0xFF; 
	return charArray;
}

int charsToInt(char *charArray) {
	int number = 0;
	number |= (charArray[0] & 0xFF) << 16;
	number |= (charArray[1] & 0xFF) << 8;
	number |= charArray[2] & 0xFF;
	return number;
}

void parentsAdd( FILE *parents, int parent, int child ){
	fseek( parents, 3 * child, SEEK_SET );
	unsigned char *parentToChars = intToChars( parent );
	fprintf( parents, "%c%c%c", parentToChars[0], parentToChars[1], parentToChars[2] );
	free( parentToChars );
}

int parentsGet( FILE *parents, int child ){
	fseek( parents, 3 * child, SEEK_SET );
	unsigned char parentToChars[3];
	fscanf( parents, "%c%c%c", &parentToChars[0], &parentToChars[1], &parentToChars[2]);
	return charsToInt( parentToChars );
}

int BFS( maze maze ){
	
	uint16_t *isVisited = calloc( maze->row * maze->col, sizeof(uint16_t) );
	if (isVisited == NULL) {
		fprintf(stderr, "Nie udalo sie zaalokowac pamieci na tablice odwiedzonych!\n");
		return -1;
	}

	int begin = maze->col * maze->start_y + maze->start_x;
	int end = maze->col * maze->end_y + maze->end_x;

	queue_t queue = queueInit( maze->row * maze->col );
	queueAdd( queue, begin );

	FILE *parents = parentsInit( maze->row * maze->col );

	int n = 0;

	while( true )
	{
		n = queuePop( queue );
		printf("%d\n", n);
		
		if( n == end ){
			break;
		}
		
		if( n == -1 ){
			break;
		}
		
		//printf("H\n");
		if( get_bit( n, isVisited ) ){
			continue;
		}
		set_bit( 1, n, isVisited );
		int nY = n / maze->col;
		int nX = n % maze->col;
		
		printf("%d\n", nX);
		printf("%d\n", nY);
		printf("\n");
		
		if( is_wall( nX, nY, nX-1, nY, maze ) == 0 ){
			int nTemp = nX-1 * maze->col + nY;
			if( !get_bit( nTemp, isVisited ) ){
				queueAdd( queue, nTemp );
				parentsAdd( parents, n, nTemp );
			}
		}
		if( is_wall( nX, nY, nX+1, nY, maze ) == 0 ){
                        int nTemp = nX+1 * maze->col + nY;
                        if( !get_bit( nTemp, isVisited ) ){
                                queueAdd( queue, nTemp );
				parentsAdd( parents, n, nTemp );
                        }
                }
		if( is_wall( nX, nY, nX, nY-1, maze ) == 0 ){
                        int nTemp = nX * maze->col + nY-1;
                        if( !get_bit( nTemp, isVisited ) ){
                                queueAdd( queue, nTemp );
				parentsAdd( parents, n, nTemp );
                        }       
                }
		if( is_wall( nX, nY, nX, nY+1, maze ) == 0 ){
                        int nTemp = nX * maze->col + nY+1;
                        if( !get_bit( nTemp, isVisited ) ){
                                queueAdd( queue, nTemp );
				parentsAdd( parents, n, nTemp );
                        }       
                }

	}

	free(isVisited);

	return n;
}















