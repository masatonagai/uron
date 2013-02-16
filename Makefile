# the compiler
CC = gcc

# compiler flags
CFLAGS = -g -Wall

SOURCES = uron.c cron.c util.c

TARGET = uron

all: $(TARGET)

uron: $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)
