EXEC1=tme6.exe
SOURCES1= key.c heap_array.c exos.c exos.h
OBJECTS1=$(SOURCES1:.c=.o)
CC=gcc
CFLAGS=-W -Wall -lm

.PHONY: default keyan

default: $(EXEC1)

key.o: key.h key.c
heap_array.o: heap_array.h heap_array.c
exos.o: exos.c key.h heap_array.h exos.h

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

$(EXEC1): $(OBJECTS1)
	$(CC) -o $@ $^ -lm

clean:
	rm -rf $(EXEC1) $(OBJECTS1) $(SOURCES1:.c=.c~) $(SOURCES1:.c=.h~) Makefile~
