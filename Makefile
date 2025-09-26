CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -O2

all: code

code: code.o
	$(CC) $(CFLAGS) -o $@ $^

code.o: code.c
	$(CC) $(CFLAGS) -c -o $@ $<

debug: CFLAGS += -g -DDEBUG
debug: code

clean:
	rm -f code code.o

.PHONY: all clean debug
