#include <stdlib.h>
#include <string.h>

#include "key.h"
#include "heap_array.h"


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
void  ajout(tas *t, key* c, int * tab) {
   resize(t, t->size + 1);
   t->a[t->size - 1] = c;
   tab[c->node] = t->size - 1;
 }

int empty(tas *t) {
  return t->size == 0;
}

/* Supprime et renvoie la clé minimale d'un tas.
   Respecte l'invariant
 */
key * delete_min(tas *t, int* index_tab) {
  if(t->a[0] != NULL){
    index_tab[t->a[0]->node] = -1;
    key *c = t->a[0];
    t->a[0] = t->a[(t->size-1)];
    index_tab[t->a[0]->node] = 0;

    t->a[(t->size-1)] = NULL;
    t->size--;

    percoler(t, 0, index_tab);
    resize(t, t->size);
    return c;
  }
}

void heap_insert(tas* T, int index_key, int * index_tab) {
    int parent = (index_key-1)/2;
    while(index_key != 0 && inf(T->a[index_key], T->a[parent])) {
      key * tmpC = T->a[index_key];
      T->a[index_key] = T->a[parent];
      T->a[parent] = tmpC;

      index_tab[T->a[index_key]->node] = index_key;
      index_tab[T->a[parent]->node] = parent;

      index_key = parent;
      parent = (index_key-1)/2;
    }
}

key* removeMin(tas *t, int* index_tab) {

  if(t->a[0] != NULL){
    key *c = t->a[0];
    t->a[0] = t->a[(t->size-1)];
    index_tab[t->a[0]->node] = -1;
    t->a[(t->size-1)] = NULL;
    t->size--;

    index_tab[t->a[0]->node] = 0;

    if (t->size > 0)
          siftDown(t, 0, index_tab);
    return c;
  }
}

void siftDown(tas* T, int nodeIndex, int* index_tab) {
    int leftChildIndex, rightChildIndex, minIndex, tmp;
    leftChildIndex = nodeIndex+1;
    rightChildIndex = nodeIndex+2;
    int heapSize = T->size;
    if (rightChildIndex >= heapSize) {
          if (leftChildIndex >= heapSize)
                return;
          else
                minIndex = leftChildIndex;
    } else {
        if(inf(T->a[leftChildIndex], T->a[rightChildIndex]))
              minIndex = leftChildIndex;
        else
              minIndex = rightChildIndex;
    }
    if(inf(T->a[minIndex], T->a[nodeIndex])){

          key * tmpC = T->a[minIndex];
          T->a[minIndex] = T->a[nodeIndex];
          T->a[nodeIndex] = tmpC;

          index_tab[T->a[minIndex]->node] = minIndex;
          index_tab[T->a[nodeIndex]->node] = nodeIndex;

          siftDown(T, minIndex, index_tab);
    }
}


/* créer un tas de faible capacité sans éléments
 * le tas retourné devra être libéré par l'appelant
 */
tas * mktas(int nb_nodes) {
  tas* t = malloc(sizeof(tas));
  t->a = malloc(nb_nodes * sizeof(key*));
  t->capacity = nb_nodes;
  t->size = 0;
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
  for(i = (t->size /2); i >= 0; i--) {
    percoler(t, i, index_tab);
  }
  return t;
}

void destroytas(tas *t) {
  free(t->a);
  free(t);
}
