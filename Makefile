#/*
# * Copyright (C) 2013 Masato Nagai
# *
# */

DESTROOT = $(DESTDIR)/usr
DESTBIN	= $(DESTROOT)/bin
DESTETC = $(DESTDIR)/etc/uron

INSTALL = install -s
CC = gcc -std=gnu99
CFLAGS = -g -O0 -Wall

HEADERS = cron.h exec.h io.h list.h path.h tag.h term.h types.h uron.h util.h
SOURCES = cron.c exec.c io.c list.c main.c tag.c term.c uron.c util.c 

.PHONY: all
all: uron

uron: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o uron $(SOURCES)

.PHONY: install
install: all
	mkdir -p $(DESTETC)
	$(INSTALL) -c -m 755 -o root uron $(DESTBIN)/

.PHONY: clean
clean:
	rm -f *.o uron
