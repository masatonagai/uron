# the compiler
CC = gcc -std=gnu99

# compiler flags
CFLAGS = -g -Wall -lcurses

SOURCES = uron.c cron.c util.c term.c

TARGET = uron

all: $(TARGET)

uron: $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)
