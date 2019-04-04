#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include<time.h>

#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <math.h>
#include "map.h"
#include "utils.h"

#include "key.h"
#include "heap_array.h"

#define NB_IT 20
#define SIZE_OF_LINE 100
#define SIZE 4
#define INFINITY_FLOAT 1.1
#define INFINITY_INT 1
#define NB_ITERATIONS 2

#define NB_ITERATIONS 2


#define IGNORE_COMMENTS char is_comment;        \
  sscanf(line, "%c", &is_comment);              \
  if(is_comment == '#'){                        \
    continue;                                   \
  }                                             \

#define MIN(a, b)((((float)a) < ((float)b)) ? ((float)a) : ((float)b))
#define MAX(a, b)((((float)a) > ((float)b)) ? ((float)a) : ((float)b))

int nbEdgesAndNodes(char* path, int * max_node){
 int fd = open(path, O_RDWR);

 int size = INT_MAX-1, nbRead, lastIndice = 0, indice=0;
 unsigned long long nbEdge=0, nbNode=0;
 char* buffer = (char*) malloc(sizeof(char)*size);
 char** words;
 char *line;

 map_int_t hash;
 map_init(&hash);
 while(1){
  //EOF
  if((nbRead=read(fd,buffer,size))==0)
   break;

  //count number of \n
  for(int i=0; i<nbRead;i++){

   if(buffer[i]=='\n'){

    //build a line
    line = (char*) malloc(sizeof(char)*(i-lastIndice));
    indice=0;
    for(int j=lastIndice; j<i;j++,indice++)
     line[indice]=buffer[j];
    line[indice]='\0';
    lastIndice=i+1;

    // split the words
    words = splitWords(line);

    // add to hashmap if it is not in
    for(int h=0;h<2;h++){
     if (map_get(&hash, words[h])==NULL) {
       if(*max_node<atoi(words[h])){
         *max_node = atoi(words[h]);
       }
      map_set(&hash, words[h], atoi(words[h]));
      nbNode++;
     }
    }
    nbEdge++;

    //free
    free(line);
    for(char** w = words; *w;w++)
     free(*w);
    free(words);
   }

  }
 }

 printf("File name : %s\nNumber of nodes : %llu\nNumber of edges : %llu\n", path, nbNode, nbEdge);

 free(buffer);
 map_deinit(&hash);
 close(fd);
 return nbNode;
}

int notisvalueinarray(int *arr, int val, int size){
    int i;
    for (i=0; i < size; i++) {
        if (arr[i] == val)
            return 0;
    }
    return 1;
}

tas* create_heap(char * ENTREE, int*index_tab, int nb_nodes) {
  FILE * f_in;
  char line[SIZE_OF_LINE];
  if ((f_in = fopen(ENTREE, "r")) == NULL) {
    fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", ENTREE);
    return 0;
  }

  int i = 0, j=0;
  int nb_ajout_key=0;

  // init du tas
  tas* heap = mktas(nb_nodes);

  fseek(f_in, 0, SEEK_SET);
  int d=1;
  while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
    IGNORE_COMMENTS;
    sscanf(line, "%d %d", & i, & j);
    // nb_ajout_key > pour l'enmplacement de l'element a ajouter dans le tas (le prochaine emplacement vide)
    // pour le noeud i
    if(index_tab[i]==-1){
			ajout(heap, parse_key(i, 1), index_tab);
			index_tab[i]=nb_ajout_key; // garder l'emplacement dans le tas
			nb_ajout_key++;
      heap->a[index_tab[i]]->voisins[heap->a[index_tab[i]]->degree -1] = j;

      heap->a[index_tab[i]]->nb_voisins =1;
		}else{
			//le degre du noeud de numero stocké dans la case u du tableau
        heap->a[index_tab[i]]->degree = heap->a[index_tab[i]]->degree+1;
        heap->a[index_tab[i]]->voisins = realloc(heap->a[index_tab[i]]->voisins, heap->a[index_tab[i]]->degree*sizeof(int));
        heap->a[index_tab[i]]->voisins[heap->a[index_tab[i]]->degree -1] = j;
        heap->a[index_tab[i]]->nb_voisins ++;
		}
    // pour le noeud j
    if(index_tab[j]==-1){
			ajout(heap, parse_key(j, 1), index_tab);
			index_tab[j]=nb_ajout_key;
			nb_ajout_key++;
      heap->a[index_tab[j]]->voisins[heap->a[index_tab[j]]->degree -1] = i;
      heap->a[index_tab[j]]->nb_voisins++;
		}else{
			//le degre du noeud de numero stocké dans la case v du tableau
      heap->a[index_tab[j]]->degree = heap->a[index_tab[j]]->degree+1;
      heap->a[index_tab[j]]->voisins = realloc(heap->a[index_tab[j]]->voisins, heap->a[index_tab[j]]->degree*sizeof(int));
      heap->a[index_tab[j]]->voisins[heap->a[index_tab[j]]->degree -1] = i;
      heap->a[index_tab[j]]->nb_voisins++;
		}
    d++;
  }
  heap = consiter(heap->a, heap->size, index_tab);
  return heap;
}

int* k_core(tas* heap, int * index_tab, int* N){
  int i = heap->size;
  int c = 0;
  unsigned int j = 0;
  key * v ;

  while(!empty(heap)){
    v = heap->a[0];
    c = MAX(c,v->degree);
    v->degree = c;
    for(j=0; j<v->nb_voisins;j++){
      if(index_tab[v->voisins[j]]==-1){
        continue;
      }
      heap->a[index_tab[v->voisins[j]]]->degree --;
      percoler(heap, index_tab[v->voisins[j]], index_tab);
    }
    supprmin(heap, index_tab);
    N[v->node]=i;
    i--;
  }
  printf("c=%d\n", c);
  printf("Fin K-Core\n");
  free(heap);
  return N;
}

int average_degree_density(char * ENTREE, int *N, int nb_nodes){
  int u=0;
  int *degree_density = (int*)malloc(sizeof(int)*(nb_nodes));

  for(u=0; u<=nb_nodes; u++){
    degree_density[u] = 0;
  }

  for(u=0; u<=nb_nodes; u++){
    degree_density[N[u]] = 0;
  }
  degree_density[0] = 0;

  FILE * f_in;
  char line[SIZE_OF_LINE];
  if ((f_in = fopen(ENTREE, "r")) == NULL) {
    fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", ENTREE);
    return 0;
  }

  fseek(f_in, 0, SEEK_SET);
  int i=0,j=0;
  while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
    IGNORE_COMMENTS;
    sscanf(line, "%d %d", & i, & j);
    if(N[i]>N[j]){
      degree_density[N[i]]++;
    }else{
      degree_density[N[j]]++;
    }
  }
  for(u=1; u<nb_nodes;u++){
    degree_density[u] = degree_density[u] + degree_density[u-1];
  }

  float max_density = 0.0, tmp_max=0.0;
  float index_max_density = 0.0;
  int val_max_density=0;

  for(u=1; u<nb_nodes;u++){
    tmp_max = (degree_density[u]/(float)u);
    if(max_density<tmp_max){
      index_max_density = degree_density[u]/(float)(u*(u-1));
      max_density = tmp_max;
      val_max_density = u;
    }
  }
  printf("Average degree densit=%f\nEdge density=%f\nSize of a densest core ordering prefix=%d\n", max_density, index_max_density, val_max_density);
  return 1;
}


// exo 3
int * MKSCORE(char * ENTREE, int nb_iterations, int max_node){
  int t=0;
  int i=0,j=0;
  int *r = (int*)malloc(sizeof(int)*(max_node+1));

  FILE * f_in;
  char line[SIZE_OF_LINE];
  if ((f_in = fopen(ENTREE, "r")) == NULL) {
    fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", ENTREE);
    return 0;
  }

  for(i=0; i<max_node; i++){
    r[i] = 0;
    r[j] = 0;
  }

  for(t=0; t<nb_iterations; t++){
    // a changer !!!
    fseek(f_in, 0, SEEK_SET);
    while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
      IGNORE_COMMENTS;
      sscanf(line, "%d %d", & i, & j);
      if(r[i] <= r[j]){
        r[i]++;
      }else{
        r[j]++;
      }
    }
  }
  for(i=0; i<max_node; i++){
    r[i] /=t;
  }
  return r;
}

void exo1(char * in_put, int nb_nodes, int max_node){
  int i = 0;
  int *index_tab = (int*)malloc(sizeof(int)*(max_node+1));
  int *N = (int*)malloc(sizeof(int)*(max_node+1));

  for(i = 0; i < (max_node+1); i++){
    index_tab[i] = -1;
    N[i] = -1;
  }
  printf("max_node=%d\n",max_node);

  tas* heap = create_heap(in_put,index_tab, nb_nodes);
  k_core(heap, index_tab, N); // on free la heap dnas k_nore

  free(index_tab);

  average_degree_density(in_put, N, nb_nodes);
  free(N);
}

int main(int argc, char *argv[]) {
  double temps;
  int nb_nodes = 0, max_node=0;
  char * in_put = argv[1];

  nb_nodes = nbEdgesAndNodes(in_put, &max_node);
  nb_nodes ++; // je sais pas pk mais quand je l'enlever ça fait malloc(): memory corruption

  clock_t start = clock();
  exo1(in_put, nb_nodes, max_node);

  // exo3
  // MKSCORE(argv[1], NB_ITERATIONS, max_node);

  temps = (double)(clock()-start)/(double)CLOCKS_PER_SEC;
  printf("\nRun terminée en %.10f seconde(s)!\n", temps);
  return 0;
}
