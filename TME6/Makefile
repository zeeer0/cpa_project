EXEC1=tme6exo1.exe
# EXEC2=tme6exo3.exe
SOURCES1= key.c heap_array.c exo1.c map.c utils.c
# SOURCES2 = exo3.c
OBJECTS1=$(SOURCES1:.c=.o)
# OBJECTS2=$(SOURCES2:.c=.o)
CC=gcc
CFLAGS=-W -Wall -lm

.PHONY: default keyan

default: $(EXEC1)

key.o: key.h key.c
map.o: map.h map.c
utils.o: utils.h utils.h
heap_array.o: heap_array.h heap_array.c
exo1.o: exo1.c key.h heap_array.h
# exo3.o: exo3.c

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

$(EXEC1): $(OBJECTS1)
	$(CC) -o $@ $^ -lm

# $(EXEC2): $(OBJECTS2)
# 	$(CC) -o $@ $^ -lm

clean:
	rm -rf $(EXEC1) $(OBJECTS1) $(SOURCES1:.c=.c~) $(SOURCES1:.c=.h~) Makefile~
	# rm -rf $(EXEC2) $(OBJECTS2) $(SOURCES2:.c=.c~) $(SOURCESZ:.c=.h~) Makefile~
