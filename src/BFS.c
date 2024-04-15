#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "maze.h"
#include "txt_management.h"
#include "BFS.h"
#include "find_path.h"

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
}

int intParentsGet( FILE *parents, int child ){

	fseek( parents, 3 * child, SEEK_SET );
	unsigned char parentToChars[3];
	fscanf( parents, "%c%c%c", &parentToChars[0], &parentToChars[1], &parentToChars[2] );
	return charsToInt( parentToChars );
}

int BFS( maze maze, char* bin_file ){
	
	uint16_t *isVisited = calloc( maze->row * maze->col, sizeof(uint16_t) );
	if (isVisited == NULL) {
		fprintf(stderr, "Nie udalo sie zaalokowac pamieci na tablice odwiedzonych!\n");
		return -1;
	}

	int begin = maze->col * maze->start_y + maze->start_x;
	int end = maze->col * maze->end_y + maze->end_x;

	//queue_tt queue = queueInit( maze->row * maze->col ); //---------------------------PODMIANKA---------------------------//
	queue_head queue = init_queue();

	//queueAdd( queue, begin ); //---------------------------PODMIANKA---------------------------//
	add_to_queue( begin, queue );

	FILE *parents = charFileInit( maze->row * maze->col, "parents.txt" );

	int n = 0;

	while( true )
	{
		n = get_from_queue( queue );
		
		if( n == end ){
			break;
		}
		
		if( get_bit( n, isVisited ) ){
			continue;
		}
		set_bit( 1, n, isVisited );
		int nY = n / maze->col;
		int nX = n % maze->col;
		
		if( is_wall( nX, nY, nX-1, nY, maze ) == 0 ){
			int nTemp = (nY) * maze->col + nX-1;
			if( !get_bit( nTemp, isVisited ) ){
				//queueAdd( queue, nTemp ); //---------------------------PODMIANKA---------------------------//
				add_to_queue( nTemp, queue );
				parentsAdd( parents, n, nTemp );
			}
		}
		if( is_wall( nX, nY, nX+1, nY, maze ) == 0 ){
                        int nTemp = (nY) * maze->col + nX+1;
                        if( !get_bit( nTemp, isVisited ) ){
                               // queueAdd( queue, nTemp ); //---------------------------PODMIANKA---------------------------//
							   add_to_queue( nTemp, queue );
				parentsAdd( parents, n, nTemp );
                        }
                }
		if( is_wall( nX, nY, nX, nY-1, maze ) == 0 ){
                        int nTemp = (nY-1) * maze->col + nX;
                        if( !get_bit( nTemp, isVisited ) ){
                                //queueAdd( queue, nTemp ); //---------------------------PODMIANKA---------------------------//
								add_to_queue( nTemp, queue );
				parentsAdd( parents, n, nTemp );
                        }       
                }
		if( is_wall( nX, nY, nX, nY+1, maze ) == 0 ){
                        int nTemp = (nY+1) * maze->col + nX;
                        if( !get_bit( nTemp, isVisited ) ){
                                //queueAdd( queue, nTemp ); //---------------------------PODMIANKA---------------------------//
								add_to_queue( nTemp, queue );
				parentsAdd( parents, n, nTemp );
                        }       
                }
	}

	free(isVisited);

	free_queue( queue );
	
	int length = pathLength( parents, begin, end );
	char *path = pathToFile( parents, begin, end, length );
	pathConvert( path, maze, length );
	pathConvertToBin( path, maze, length, bin_file );
	
	fclose( parents );

	return n;
}

int pathLength( FILE *parents, int begin, int end ){
	
	int parent = end;
	int pathLength = 0;
	
	while( parent != begin ){
		
		pathLength++;
		int child = parent;
		parent = intParentsGet( parents, child );
	}
	
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
			break;
		}
		fseek( path, length * 3 - i, SEEK_SET );
		unsigned char *parentToChars = intToChars( parent );
		fprintf( path, "%c%c%c", parentToChars[0], parentToChars[1], parentToChars[2] );
		
		int child = parent;
		parent = intParentsGet( parents, child );
		i += 3;
	}
	
	fclose( path );
	
	return "path.txt";
}

void pathConvert( char *ogPath, maze maze, int size ){
	
	unsigned char charArray[3];
	FILE *path = fopen( ogPath, "r" );
	
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
	int count = 0;

	printf( "%s\n", "START" );
	
	for(int i = 1; i <= size; i++){
	
		first = second;
		fseek( path, 3 * i, SEEK_SET );
		fscanf( path, "%c%c%c", &charArray[0], &charArray[1], &charArray[2] );
		second = charsToInt( charArray );
		
		firstDirection = secondDirection;
		secondDirection = second - first;
		
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
		
		if( firstDirection != secondDirection ){
			
			if( count != 0){
				printf( "%s %d\n", "FORWARD", count );
				count = 1;
			} 

			if( secondDirection - firstDirection == 1 || firstDirection == 4 && secondDirection == 1 ){
				printf( "%s\n", "TURNRIGHT" );
			}
			if( secondDirection - firstDirection == -1 || firstDirection == 1 && secondDirection == 4 ){
				printf( "%s\n", "TURNLEFT" );
			}
		} else{

			count++;
		}
	}

	printf( "%s %d\n", "FORWARD", count );

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

		if( secondDirection - firstDirection == 1 || firstDirection == 4 && secondDirection == 1 ){
				printf( "%s\n", "TURNRIGHT" );
		}
		if( secondDirection - firstDirection == -1 || firstDirection == 1 && secondDirection == 4 ){
				printf( "%s\n", "TURNLEFT" );
		}

		printf( "%s\n", "END" );
	
	fclose( path );
}

int pathConvertToBin( char *ogPath, maze maze, int size, char* bin_file ) {

    FILE* f = fopen(bin_file, "rb+");
    uint32_t offset_to_solution, is_offset;
    uint32_t steps;
    int steps_offset;
    //int next, node, len, direction;
    if (f == NULL) {
        fprintf(stderr, "Nie udalo sie otworzyc pliku binarnego do zapisu sciezki\n");
        return -1;
    }
    fseek(f, 0, SEEK_END);
    offset_to_solution = ftell(f) - 4;
    fseek(f, 33, SEEK_SET);
    fread(&is_offset, sizeof is_offset, 1, f);
    if (is_offset != 0) {
        fprintf(stderr, "W pliku binarnym jest juz zapisana sciezka\n");
        return -2;
    }
    fseek(f, 33, SEEK_SET);
    fwrite(&offset_to_solution, sizeof offset_to_solution, 1, f);
    fseek(f, 0, SEEK_END);
    steps_offset = ftell(f);
    steps = 0;
    fwrite(&steps, sizeof steps, 1, f);

    unsigned char charArray[3];
	FILE *path = fopen( ogPath, "r" );
	
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
	int count = 0;
	
	for(int i = 1; i <= size; i++){
	
		first = second;
		fseek( path, 3 * i, SEEK_SET );
		fscanf( path, "%c%c%c", &charArray[0], &charArray[1], &charArray[2] );
		second = charsToInt( charArray );
		
		firstDirection = secondDirection;
		secondDirection = second - first;
		
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
		
		if( firstDirection != secondDirection ){
			
			if( count == 0){
				continue;
			} 
			steps++;

			uint8_t countBin = count - 1;
			uint8_t direction;

			if( firstDirection == 1 ){
				direction = 'N';
			}
			if( firstDirection == 2 ){
				direction = 'E';
			}
			if( firstDirection == 3 ){
				direction = 'S';
			}
			if( firstDirection == 4 ){
				direction = 'W';
			}

			fwrite( &direction, 1, 1, f );
			fwrite( &countBin, 1, 1, f );

			count = 1;

		} else{

			count++;
		}
	}

	firstDirection = secondDirection;

	uint8_t countBin = count - 1;
	uint8_t direction;

	if( firstDirection == 1 ){
		direction = 'N';
	}
	if( firstDirection == 2 ){
		direction = 'E';
	}
	if( firstDirection == 3 ){
		direction = 'S';
	}
	if( firstDirection == 4 ){
		direction = 'W';
	}

	fwrite( &direction, 1, 1, f );
	fwrite( &countBin, 1, 1, f );

	fseek(f, steps_offset, SEEK_SET);
    steps;
    fwrite(&steps, sizeof steps, 1, f);
    fclose(f);

	return 0;
}