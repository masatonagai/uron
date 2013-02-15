# the compiler
CC = gcc

# compiler flags
CFLAGS = -g -Wall

SOURCES = uron.c cron.c

TARGET = uron

all: $(TARGET)

uron: $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)
