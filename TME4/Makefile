EXEC=tme4.exe
SOURCES=main.c vector.c
OBJECTS=$(SOURCES:.c=.o)
CC=gcc
CFLAGS=-W -Wall -lm
 
.PHONY: default clean
 
default: $(EXEC)
 

# utils.o: utils.c utils.h
# exos.o: exos.c exos.h utils.h
vector.o:vector.c vector.h
main.o: main.c 
#exos.h utils.h map.h


%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)
 
$(EXEC): $(OBJECTS)
	$(CC) -o $@ $^ -lm
 
clean:
	rm -rf $(EXEC) $(OBJECTS) $(SOURCES:.c=.c~) $(SOURCES:.c=.h~) Makefile~
