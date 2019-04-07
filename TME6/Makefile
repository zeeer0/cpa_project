EXEC1=tme6.exe
SOURCES1= key.c heap_array.c exos.c utils.c
OBJECTS1=$(SOURCES1:.c=.o)
CC=gcc
CFLAGS=-W -Wall -lm

.PHONY: default keyan

default: $(EXEC1)

key.o: key.h key.c
utils.o: utils.h utils.h
heap_array.o: heap_array.h heap_array.c
exos.o: exos.c key.h heap_array.h

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

$(EXEC1): $(OBJECTS1)
	$(CC) -o $@ $^ -lm

clean:
	rm -rf $(EXEC1) $(OBJECTS1) $(SOURCES1:.c=.c~) $(SOURCES1:.c=.h~) Makefile~
	# rm -rf $(EXEC2) $(OBJECTS2) $(SOURCES2:.c=.c~) $(SOURCESZ:.c=.h~) Makefile~
