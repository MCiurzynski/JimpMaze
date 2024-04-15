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
			fprintf(stderr, "Kolejce zabraklo miejsca!\n");
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

FILE * charFileInit( int size, char* fileName ){

	FILE *file = fopen( fileName, "w+" );
	if( file == NULL ){
		fprintf( stderr, "Nie udalo sie utworzyc pliku \"%s\"!\n", fileName );
                return NULL;
                
	} else{
		for( int i = 0; i < size; i++ ){
			fprintf( file, "%c%c%c", 255, 255, 255 );
		}
		return file;
		
	}
}

unsigned char* intToChars(int number) {
	unsigned static char charArray[3];
	charArray[0] = (number >> 16) & 0xFF;
	charArray[1] = (number >> 8) & 0xFF;
	charArray[2] = number & 0xFF; 
	return charArray;
}

int charsToInt(unsigned char *charArray) {
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
	//free( parentToChars );
}

int intParentsGet( FILE *parents, int child ){
	fseek( parents, 3 * child, SEEK_SET );
	unsigned char parentToChars[3];
	fscanf( parents, "%c%c%c", &parentToChars[0], &parentToChars[1], &parentToChars[2] );
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

	//FILE *parents = parentsInit( maze->row * maze->col );
	FILE *parents = charFileInit( maze->row * maze->col, "parents.txt" );

	int n = 0;

	while( true )
	{
		n = queuePop( queue );
		//printf("%d\n", n);
		
		if( n == end ){
			//printf("Licze na glitche: %d\n", intParentsGet( parents, n ));
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
		
		//printf("%d\n", nX);
		//printf("%d\n", nY);
		//rintf("\n");
		
		if( is_wall( nX, nY, nX-1, nY, maze ) == 0 ){
			int nTemp = (nY) * maze->col + nX-1;
			if( !get_bit( nTemp, isVisited ) ){
				queueAdd( queue, nTemp );
				parentsAdd( parents, n, nTemp );
			}
		}
		if( is_wall( nX, nY, nX+1, nY, maze ) == 0 ){
                        int nTemp = (nY) * maze->col + nX+1;
                        if( !get_bit( nTemp, isVisited ) ){
                                queueAdd( queue, nTemp );
				parentsAdd( parents, n, nTemp );
                        }
                }
		if( is_wall( nX, nY, nX, nY-1, maze ) == 0 ){
                        int nTemp = (nY-1) * maze->col + nX;
                        if( !get_bit( nTemp, isVisited ) ){
                                queueAdd( queue, nTemp );
				parentsAdd( parents, n, nTemp );
                        }       
                }
		if( is_wall( nX, nY, nX, nY+1, maze ) == 0 ){
                        int nTemp = (nY+1) * maze->col + nX;
                        if( !get_bit( nTemp, isVisited ) ){
                                queueAdd( queue, nTemp );
				parentsAdd( parents, n, nTemp );
                        }       
                }

	}

	free(isVisited);
	
	int length = pathLength( parents, begin, end );
	char *path = pathToFile( parents, begin, end, length );
	pathConvert( path, maze, length );
	
	fclose( parents );

	return n;
}

int pathLength( FILE *parents, int begin, int end ){
	
	int parent = end;
	int pathLength = 0;
	
	while( parent != begin ){
		
		pathLength++;
		//printf( "Węzeł: %d\n", parent );
		int child = parent;
		parent = intParentsGet( parents, child );
	}
	
	//printf( "Węzeł: %d\n", begin );
	//printf( "Długość ścieżki samotności: %d\n", pathLength ); 
	
	return pathLength;
}

char *pathToFile( FILE *parents, int begin, int end, int length ){
	
	int parent = end;
	FILE *path = charFileInit( length + 1, "path.txt" );
	int i = 0;
	
	while( true ){
		
		if( begin == parent ){
			fseek( path, 0, SEEK_SET );
			unsigned char *parentToChars = intToChars( begin );
			fprintf( path, "%c%c%c", parentToChars[0], parentToChars[1], parentToChars[2] );
			//free( parentToChars );
			break;
		}
		//printf( "Węzeł: %d\n", parent );
		fseek( path, length * 3 - i, SEEK_SET );
		unsigned char *parentToChars = intToChars( parent );
		fprintf( path, "%c%c%c", parentToChars[0], parentToChars[1], parentToChars[2] );
		
		int child = parent;
		parent = intParentsGet( parents, child );
		i += 3;
		//free( parentToChars );
	}
	
	//fileTest( path, length );
	
	fclose( path );
	
	return "path.txt";

}

void fileTest( FILE *path, int length ){

	fseek( path, 0, SEEK_SET );
	int j = 0;
	for(int i = 0; i <= length; i++){
		unsigned char parentToChars[3];
		fscanf( path, "%c%c%c", &parentToChars[0], &parentToChars[1], &parentToChars[2] );
		printf( "%d. %d\n", i, charsToInt( parentToChars ) );
		j+=3;
		fseek( path, j, SEEK_SET );
	}
}

void pathConvert( char *ogPath, maze maze, int size){
	
	unsigned char charArray[3];
	FILE *path = fopen( ogPath, "r" );
	FILE *convertedPath = fopen( "convertedPath.txt", "w+" );
	
	fseek( path, 0, SEEK_SET );
	fscanf( path, "%c%c%c", &charArray[0], &charArray[1], &charArray[2] );
	int second = charsToInt( charArray );
	int first = 0;
	
	int firstDirection = 0;
	int secondDirection = 0;
	if( maze->start_direction == 'S' ){
		secondDirection = 1;
	}
	if( maze->start_direction == 'W' ){
		secondDirection = 2;
	}
	if( maze->start_direction == 'N' ){
		secondDirection = 3;
	}
	if( maze->start_direction == 'E' ){
		secondDirection = 4;
	}
	//secondDirection = 2;
	int count = 0;
	
	for(int i = 1; i <= size; i++){
	
		first = second;
		fseek( path, 3 * i, SEEK_SET );
		fscanf( path, "%c%c%c", &charArray[0], &charArray[1], &charArray[2] );
		second = charsToInt( charArray );
		//printf( "First: %d\n", first );
		//printf( "Second: %d\n\n", second );
		
		firstDirection = secondDirection;
		secondDirection = second - first;
		//printf( "firstDirection: %d\n", firstDirection );
		
		if( secondDirection == 1 ){
			secondDirection = 2;
		}
		if( secondDirection == -1 ){
			secondDirection = 4;
		}
		if( secondDirection ==  maze->col ){
			secondDirection = 3;
		}
		if( secondDirection == -1 * maze->col ){
			secondDirection = 1;
		}
		//printf( "secondDirection: %d\n\n", secondDirection );
		
		
		if( firstDirection != secondDirection ){
			
			if( count != 0){
				fprintf( convertedPath, "%s %d\n", "FORWARD", count );
				count = 1;
			} else{
				fprintf( convertedPath, "%s\n", "START");
				count = 1;
			}
	
			if( secondDirection - firstDirection == 1 || firstDirection == 4 && secondDirection == 1 ){
				fprintf( convertedPath, "%s\n", "TURNRIGHT" );
			}
			if( secondDirection - firstDirection == -1 || firstDirection == 1 && secondDirection == 4 ){
				fprintf( convertedPath, "%s\n", "TURNLEFT" );
			}
		} else{
			count++;
		}
		
		//printf( "Count: %d\n\n", count );	
	}

	fprintf( convertedPath, "%s %d\n", "FORWARD", count );
		firstDirection = secondDirection;
		if( maze->end_direction == 'S' ){
			secondDirection = 3;
		}
		if( maze->end_direction == 'W' ){
			secondDirection = 4;
		}
		if( maze->end_direction == 'N' ){
			secondDirection = 1;
		}
		if( maze->end_direction == 'E' ){
			secondDirection = 2;
		}

		if( firstDirection == 1 && secondDirection == 4 ){
			fprintf( convertedPath, "%s\n", "TURNLEFT" );
		}
		if( firstDirection == 4 && secondDirection == 1 ){
			fprintf( convertedPath, "%s\n", "TURNRIGHT" );
		} 
		if( secondDirection - firstDirection == 1 ){
			fprintf( convertedPath, "%s\n", "TURNRIGHT" );
		}
		if( secondDirection - firstDirection == -1 ){
			fprintf( convertedPath, "%s\n", "TURNLEFT" );
		}

		fprintf( convertedPath, "%s\n", "END" );
	
	fclose( path );
	fclose( convertedPath );
}














