all: 
	cc -Wall --pedantic src/*.c -o bin/main.out

test:
	bin/main.out dane/maze

clean:
	rm -f bin/*
