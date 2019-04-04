#include <stdlib.h>
#include <string.h>

#include "key.h"
#include "heap_array.h"

#define PARENT(n) ( ((n) / 2) )
#define LEFT(n) ( (n) * 2 )
#define RIGHT(n) ( (n) * 2 + 1 )


/* Maintient l'invariant des tas:
   Si les sous-tas enracinés en LEFT(i) et RIGHT(i) satisfont l'invariant,
   Alors après l'exécution le sous-tas enraciné en i le satisfait.
 */
void percoler(tas* t, size_t i, int* index_tab) {
  size_t l = (2*i)+1;
  size_t r = (2*i)+2;
  size_t min = i;

  if(l < t->size && inf(t->a[l], t->a[i])){
    min = l;
  }
  if(r < t->size && inf(t->a[r], t->a[min])){
    min = r;
  }
  if(min != i) {
    key* tmp = t->a[min];
    t->a[min] = t->a[i];
    t->a[i] = tmp;

    index_tab[t->a[min]->node] = min;
    index_tab[t->a[i]->node] = i;

    percoler(t, min, index_tab);

  }
}


void resize(tas *t, size_t newsize) {
  t->size = newsize;
  if(newsize > t->capacity) {
    t->a = realloc(t->a, t->capacity * sizeof(key*) * 2);
    t->capacity = t->capacity * 2;
  }
}


/* Ajoute un élément à un tas en respectant l'invariant.
 */
 void  ajout(tas *t, key* c, int* tab) {
   resize(t, t->size + 1);
   t->a[t->size - 1] = c;
   tab[c->node] = t->size - 1;
 }

key* mintas(tas *t) {
  return t->a[0];
}

int empty(tas *t) {
  return t->size == 0;
}

/* Supprime et renvoie la clé minimale d'un tas.
   Respecte l'invariant
 */
key * supprmin(tas *t, int* index_tab) {
  if(t->a[0] != NULL){
    index_tab[t->a[0]->node] = -1;
    key *c = t->a[0];
    t->a[0] = t->a[(t->size-1)];
    t->size--;

    percoler(t, 0, index_tab);
    resize(t, t->size);
    return c;
  }
}


/* créer un tas de faible capacité sans éléments
 * le tas retourné devra être libéré par l'appelant
 */
tas * mktas(int nb_nodes) {
  tas* t = malloc(sizeof(tas));
  t->a = malloc((nb_nodes+1) * sizeof(key*));
  t->capacity = nb_nodes+1;
  t->size = 0;
  t->last = 0;
  return t;
}

/* Crée un tas à partir d'un tableau de clé,
   en utilisant un tableau de clé pour le stockage.
   la fonction prend le controle de c.
   les clés ne sont pas copiées.
   Le tas doit être détruit par l'appelant
*/
tas * consiter(key** c, size_t size, int * index_tab) {
  tas *t = malloc(sizeof(tas));
  int i=0;
  t->a = c;
  t->size = size;
  t->capacity = size;
  for(i = t->size / 2; i >= 0; i--) {
    percoler(t, i, index_tab);
  }
  return t;
}

void destroytas(tas *t) {
  free(t->a);
  free(t);
}

void print_heap(tas* heap){
  unsigned int i=0;
  for(i=0; i<heap->size; i++){
    printf("node[%d]=%d \t degree=%d\n",i,heap->a[i]->node, heap->a[i]->degree);
  }
}
