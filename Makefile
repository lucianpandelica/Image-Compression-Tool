CC = gcc
CFLAGS = -g -Wall -lm

build: quadtree

quadtree: main.c header.c header.h
	$(CC) main.c header.c -o quadtree $(CFLAGS)

clean:
	rm -f quadtree
	rm -f *.out