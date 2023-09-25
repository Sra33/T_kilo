CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -I kilo.h
DFLAGS = -g -Wall -Wextra -pedantic -std=c99 -I kilo.h

kilo: kilo.c kilo.h
	$(CC) kilo.c -o kilo $(CFLAGS)
clean:
	rm -rf kilo
dbug:
	$(CC) kilo.c -o kilo $(DFLAGS)

