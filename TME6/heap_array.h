#include "key.h"
#include <stdlib.h>

typedef struct {
  unsigned int size;
  size_t capacity;
  key** a;
  unsigned last;
} tas;

void ajout(tas *t, key* c, int * index_tab);

int empty(tas *t);

key* mintas(tas *t);

key * supprmin(tas *t, int* index_tab);

tas * mktas(int nb_nodes);

tas * consiter(key** c, size_t size, int* index_tab);

void destroytas(tas *t);

void print_heap(tas* heap);
