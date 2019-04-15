
CC=gcc
CFLAGS=-Wall
LIBS= -pthread -lcdsl -L./../synch_implementations -I./../synch_implementations

targets: dijkstra original

dijkstra: dijkstra.c
    $(CC) dijkstra.c -o dijkstra.out $(LIBS)

original: dijkstra.orig.c
    $(CC) dijkstra.orig.c -o original.out $(LIBS)

clean:
    rm *.out
    rm core

