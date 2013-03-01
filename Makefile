# the compiler
CC = gcc -std=gnu99

# compiler flags
CFLAGS = -g -O0 -Wall

SOURCES = src/*.c

TARGET = uron

all: $(TARGET)

uron: $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)
