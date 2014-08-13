CFLAGS = -DLSB_FIRST -ggdb -Wall
CC = gcc

TARGET = 6502

${TARGET}:	M6502.o main.o tty.o memory_map.h Makefile
		$(CC) main.o tty.o M6502.o -o $@

M6502.o:	M6502.c Tables.h M6502.h Codes.h
		gcc -c M6502.c -o $@

clean:
		rm -f *.o ${TARGET}
