CC=gcc
CFLAGS=-ansi -Wall
LIBS=-lcurl

logAndReg: logAndReg.c 
	$(CC) -o logAndReg $(CFLAGS) logAndReg.c $(LIBS)
