# the compiler
CC = gcc -std=gnu99

# compiler flags
CFLAGS = -g -O0 -Wall -lcurses

SOURCES = uron.c cron.c util.c term.c io.c list.c tag.c

TARGET = uron

all: $(TARGET)

uron: $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)
