#include <stdlib.h>
#include <stdio.h>
#include "key.h"


key* parse_key(int node, int degree) {
  key* res = (key*) malloc(sizeof(key));
  res->node = node;
  res->degree = degree;
  res->core = -1;
  res->neighbors = (int*)calloc(degree, sizeof(int));
  res->nb_neighbors = 0;
  return res;
}

int eg(key* a, key* b) {
  if(a->degree != b->degree)
      return 0;
  return 1;
}

int inf(key* a, key* b) {
  return a->degree < b->degree;
}

void print(key * c){
   printf("node %u", c->node);
   printf("degree %u\n", c->degree);
}
