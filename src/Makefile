OBJS = queue.o comparison.o
CC = gcc
CFLAGS = -Wall -c
LFLAGS = -Wall

all : comparison

comparison : $(OBJS)
		$(CC) $(LFLAGS) $(OBJS) -o comparison

queue.o : process.h queue.h queue.c
		$(CC) $(CFLAGS) queue.c

comparison.o : process.h queue.h comparison.h comparison.c
		$(CC) $(CFLAGS) comparison.c

clean:
		\rm -f *.o comparison

srctar:
		tar czvf cscorley_src.tar.gz *.h *.c Makefile

tar: comparison
		tar czvf cscorley_build.tar.gz comparison

