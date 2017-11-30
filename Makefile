include ./Makefile.setup

CFLAGS ?= -Wall

all: test

test: ./config.mk test.c
	$(CC) $(CFLAGS)  -o $@ test.c -lEGL -lGLESv2 -lm

clean:
	rm -f test
