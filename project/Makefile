CC=gcc

SRCDIR=src
BUILDDIR=obj
BINDIR=bin

all: build

build:
	$(CC) -g -c $(SRCDIR)/main.c -o $(BUILDDIR)/main.o -lpthread 
	$(CC) -g $(BUILDDIR)/main.o -o $(BINDIR)/lavacao -lpthread

clean:
	rm $(BUILDDIR)/*.o $(BINDIR)/*
