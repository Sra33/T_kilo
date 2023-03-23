CC = gcc
CFLAG = -Wall -Wextra -pedantic -std=c99 -I ./kilo.h

kilo: kilo.c kilo.h
	$(CC) kilo.c -o kilo $(CFLAGS)
