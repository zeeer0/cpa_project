EXEC=tme3.exe
SOURCES=main.c exos.c fifo.c
OBJECTS=$(SOURCES:.c=.o)
CC=gcc
CFLAGS=-W -Wall -lm

.PHONY: default clean

default: $(EXEC)

fifo.o: fifo.c fifo.h
exos.o: exos.c exos.h fifo.h
main.o: main.c fifo.h exos.h

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

$(EXEC): $(OBJECTS)
	$(CC) -o $@ $^ -lm

clean:
	rm -rf $(EXEC) $(OBJECTS) $(SOURCES:.c=.c~) $(SOURCES:.c=.h~) Makefile~
