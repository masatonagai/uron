#/*
# * Copyright (C) 2013 Masato Nagai
# *
# */

DESTROOT = $(DESTDIR)/usr
DESTBIN	= $(DESTROOT)/bin
INSTALL = install -s
CC = gcc -std=gnu99
CFLAGS = -g -O0 -Wall

SOURCES = uron.c cron.c exec.c io.c list.c main.c tag.c term.c util.c 

all: uron

uron: $(SOURCES)
	$(CC) $(CFLAGS) -o uron $(SOURCES)

install: all
	$(INSTALL) -c -m 755 -o root uron $(DESTBIN)/
