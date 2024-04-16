all: 
	mkdir -p bin
	cc src/*.c -o bin/main.out

test:
	bin/main.out dane/maze.txt

testbin:
	bin/main.out dane/maze.bin

testbig:
	bin/main.out dane/maze-512x512.txt

testbigram:
	valgrind bin/main.out dane/maze-512x512.txt

clean:
	rm -f bin/*
