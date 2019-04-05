#include "key.h"
#include <stdlib.h>

typedef struct {
  unsigned int size;
  size_t capacity;
  key** a;
} tas;

void percoler(tas* t, size_t i, int* index_tab);

void ajout(tas *t, key* c, int * index_tab);

int empty(tas *t);

key* mintas(tas *t);

key * delete_min(tas *t, int* index_tab);

tas * mktas(int nb_nodes);

tas * consiter(key** c, size_t size, int* index_tab);

void destroytas(tas *t);
