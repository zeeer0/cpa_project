#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>

#include "map.h"
#include "utils.h"
#include "key.h"
#include "heap_array.h"

#define SIZE_OF_LINE 100
#define NB_ITERATIONS 2

#define IGNORE_COMMENTS char is_comment;        \
  sscanf(line, "%c", &is_comment);              \
  if(is_comment == '#'){                        \
    continue;                                   \
  }                                             \

#define MIN(a, b)((((float)a) < ((float)b)) ? ((float)a) : ((float)b))
#define MAX(a, b)((((float)a) > ((float)b)) ? ((float)a) : ((float)b))

/**
* Function to count the number of nodes and edges in file
*   and change the value of max_node
*
* in_put : path of file inPut
* max_node : the max number of the nodes
**/
int nbEdgesAndNodes(char* in_put, int * max_node){
 int fd = open(in_put, O_RDWR);

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

 printf("File name : %s\nNumber of nodes : %llu\nNumber of edges : %llu\n", in_put, nbNode, nbEdge);

 free(buffer);
 map_deinit(&hash);
 close(fd);
 return nbNode;
}

/**
* Fonction to create heap structure
*   and index_tab which saves node indexes in the heap
*
* in_put : path of file inPut
* index_tab : the index for each node in the heap (initialized with -1)
* nb_node : the number of nodes in the file
*
* return : heap with every node read
**/
tas* create_heap(char * in_put, int*index_tab, int nb_nodes) {
  FILE * f_in;
  char line[SIZE_OF_LINE];
  if ((f_in = fopen(in_put, "r")) == NULL) {
    fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", in_put);
    return 0;
  }

  int i = 0, j=0; // the nodes that will be read in the file
  int index_node=0; // to save the current index in our heap

  // heap initialization
  tas* heap = mktas(nb_nodes);

  fseek(f_in, 0, SEEK_SET);
  while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
    IGNORE_COMMENTS;
    sscanf(line, "%d %d", & i, & j);

    if(index_tab[i]==-1){ /*the node i does not exist in the heap*/
      // add the node in heap
      ajout(heap, parse_key(i, 1), index_tab);
      // save the node location in the index table
      index_tab[i]=index_node;
      // increment the index for nodes
			index_node++;
      // add node j as neighbors of node i
      heap->a[index_tab[i]]->neighbors[heap->a[index_tab[i]]->degree -1] = j;
      heap->a[index_tab[i]]->nb_neighbors =1;
		}else{ /*the node i exist in the heap*/
        // increment the degree of i
        heap->a[index_tab[i]]->degree ++;
        // reallocate the list of neighbors and add node j as neighbors of i
        heap->a[index_tab[i]]->neighbors = realloc(heap->a[index_tab[i]]->neighbors, heap->a[index_tab[i]]->degree*sizeof(int));
        heap->a[index_tab[i]]->neighbors[heap->a[index_tab[i]]->degree -1] = j;
        heap->a[index_tab[i]]->nb_neighbors ++;
		}

    if(index_tab[j]==-1){ /*the node j does not exist in the heap*/
      // add the node in heap
			ajout(heap, parse_key(j, 1), index_tab);
      // save the node location in the index table
			index_tab[j]=index_node;
      // increment the index for nodes
			index_node++;
      // add node i as neighbors of node j
      heap->a[index_tab[j]]->neighbors[heap->a[index_tab[j]]->degree -1] = i;
      heap->a[index_tab[j]]->nb_neighbors++;
		}else{ /*the node i exist in the heap*/
      // increment the degree of j
      heap->a[index_tab[j]]->degree++;
      // reallocate the list of neighbors and add node i as neighbors of j
      heap->a[index_tab[j]]->neighbors = realloc(heap->a[index_tab[j]]->neighbors, heap->a[index_tab[j]]->degree*sizeof(int));
      heap->a[index_tab[j]]->neighbors[heap->a[index_tab[j]]->degree -1] = i;
      heap->a[index_tab[j]]->nb_neighbors++;
		}
  }
  // building the heap structure
  heap = consiter(heap->a, heap->size, index_tab);


  return heap;
}

/**
* The order of removal of each node from the heap
*
* out_put :
* heap : heap
* index_tab : the index for each node in the heap
* N : list to save the delete order of each node from the heap
*
* return : a node list with their delete order in the heap
**/
int* k_core_decomposition(char* out_put, tas* heap, int * index_tab, int* N){
  int i = heap->size; // the current size of the heap
  int c = 0; // core
  unsigned int j = 0; // the current index
  key * v ; //  key in heap (the item that will be deleted in the heap)

  FILE * f_out;
  f_out = fopen (out_put,"w");

  while(!empty(heap)){
    v = heap->a[0];
    c = MAX(c,v->degree);
    v->core = c;
    // fprintf (f_out, "%d \t %d\n", v->core, v->degree);
    for(j=0; j<v->nb_neighbors;j++){ /*decrement the degree for each neighbor of v */
      if(index_tab[v->neighbors[j]]==-1){ /*the neighbor at the index j does not exist*/
        continue;
      }
      // decrement the degree of
      heap->a[index_tab[v->neighbors[j]]]->degree --;
      // reorganization of the heap
      percoler(heap, index_tab[v->neighbors[j]], index_tab);
    }
    // delete the min
    delete_min(heap, index_tab);
    // save the deletion order of the node v
    N[v->node]=i;
    // decrement i
    i--;
  }
  printf("Max core=%d\n", c);
  free(heap);
  return N;
}

/**
* For each subgraph, calculate
*   (i)the average degree density,
*   (ii) the edge density and
*   (iii) the size of a densest core ordering prefix1
*   and print the max for each of them
*
* in_put : path of file inPut
* N :
* nb_node : the number of nodes in the file
*
**/

int average_degree_density(char * in_put, int *N, int nb_nodes){
  float tmp_max=0.0;
  float max_density = 0.0;
  float index_max_density = 0.0;
  int val_max_density=0;

  int *degree_density = (int*)malloc(sizeof(int)*(nb_nodes));

  int u=0;
  for(u=0; u<=nb_nodes; u++){
    degree_density[u] = 0;
    degree_density[N[u]] = 0;
  }

  FILE * f_in;
  char line[SIZE_OF_LINE];
  if ((f_in = fopen(in_put, "r")) == NULL) {
    fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", in_put);
    return 0;
  }

  fseek(f_in, 0, SEEK_SET);
  int i=0,j=0;
  while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
    IGNORE_COMMENTS;
    sscanf(line, "%d %d", & i, & j);
    // compare the order of removal in the heap for i and j
    if(N[i]>N[j]){
      degree_density[N[i]]++;
    }else{
      degree_density[N[j]]++;
    }
  }

  for(u=1; u<nb_nodes;u++){
    degree_density[u] = degree_density[u] + degree_density[u-1];
    tmp_max = (degree_density[u]/(float)u);
    if(max_density<tmp_max){
      index_max_density = (2*degree_density[u])/(float)(u*(u-1));
      max_density = tmp_max;
      val_max_density = u;
    }
  }
  printf("Average degree densit=%f\nEdge density=%f\nSize of a densest core ordering prefix=%d\n", max_density, index_max_density, val_max_density);
  return 1;
}

/**
* Swap between two pointers
*
* x : val1
* y : val2
**/
void swap(int *x,int *y){
    int temp;
    temp = *x;
    *x = *y;
    *y = temp;
}


/**
* Sort a list in descending order and
*   save the order of the nodes after sorting it
*
* list : pointer list of value to sort
* r_index_node : pointer to the order of each node in the list
* m : O (first index of the list)
* n : size of list
**/
void quicksort(int* list, int *r_index_node, int m,int n){
    float key;
    int i,j,k;
    if( m < n){
      // choose_pivot
      k = ((m+n) /2);

      swap(&list[m],&list[k]);
      swap(&r_index_node[m],&r_index_node[k]);

      key = list[m];
      i = m+1;
      j = n;
      printf(">>>>>>> j=%d \n", j);
      printf("list[%d]=%f \t list[%d]=%f\n",m, list[m], j, list[j]);
      while(i <= j){
          while((i <= n) && (list[i] >= key))
              i++;
          while((j >= m) && (list[j] < key))
              j--;
          if(list[i]!=-1.0 && i < j){
            swap(&list[i],&list[j]);
            swap(&r_index_node[i],&r_index_node[j]);
          }
      }
      // swap two elements
      swap(&list[m],&list[j]);
      swap(&r_index_node[m],&r_index_node[j]);
      // recursively sort the lesser list
      quicksort(list, r_index_node,m,j-1);
      quicksort(list, r_index_node,j+1,n);
    }
}

/**
* Compute density score
*
* in_put : path of file inPut
* nb_iterations : number of iterations
* max_node : the max number of the nodes
* r :
* r_index_node :
**/
int MKSCORE(char * in_put, int nb_iterations, int max_node, float * r, int * r_index_node){
  int t=0, i=0, j=0;

  FILE * f_in;
  char line[SIZE_OF_LINE];
  if ((f_in = fopen(in_put, "r")) == NULL) {
    fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", in_put);
    return 0;
  }

  for(t=0; t<nb_iterations; t++){
    fseek(f_in, 0, SEEK_SET);
    while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
      IGNORE_COMMENTS;
      sscanf(line, "%d %d", & i, & j);
      r_index_node[i] = i;
      r_index_node[j] = j;
      if(r[i] == -1.0)
        r[i] = 0.0;
      if(r[j] == -1.0)
        r[j] = 0.0;

      if(r[i] <= r[j]){
        r[i] = r[i]+1.0;
      }else{
        r[j] = r[j]+1.0;
      }
    }
  }
  for(i=0; i<max_node; i++){
    if(r[i]==-1.0)
      continue;
    r[i]= (float)r[i]/((float)t);
  }
  return 1;
}

/**
* Launch of the first exercise
*
* in_put : path of file inPut
* out_put : path of file outPut
* nb_node : the number of nodes in the file
* max_node : the max number of the nodes
**/
void exo1(char * in_put, char* out_put, int nb_nodes, int max_node){
  int i = 0; // variable to be used in the for
  int *index_tab = (int*)malloc(sizeof(int)*(max_node+1)); // index_tab : the index for each node in the heap
  int *N = (int*)malloc(sizeof(int)*(max_node+1)); // ist to save the delete order for each node in a heap

  // initialization of both lists index_tab and N to -1
  for(i = 0; i < (max_node+1); i++){
    index_tab[i] = -1;
    N[i] = -1;
  }

  // creating the heap data structure
  tas* heap = create_heap(in_put,index_tab, nb_nodes);

  // compute the k-core decomposition
  k_core_decomposition(out_put, heap, index_tab, N); // we destroy the heap in the function

  free(index_tab);

  // compute the average degree density, the edge density
  // and the size of a densest core ordering prefix
  average_degree_density(in_put, N, nb_nodes);
  free(N);
}

/**
* Launch of the third exercise
*
* in_put : path of file inPut
* max_node : the max number of the nodes
**/
void exo3(char * in_put, int max_node){
  int j=0;  // variable to be used in the for
  float * r = (float*)malloc(sizeof(float)*(max_node+1));
  int * r_index_node = (int*)malloc(sizeof(int)*(max_node+1));

  // initialization of r_index_node and r to -1
  for(j=0; j<=max_node; j++){
    r_index_node[j] = -1;
    r[j] = -1.0;
  }

  MKSCORE(in_put, NB_ITERATIONS, max_node, r, r_index_node);
  // sort r
  quicksort(r, r_index_node,0,max_node);

  // compute the average degree density, the edge density
  // and the size of a densest core ordering prefix
  average_degree_density(in_put, r_index_node, max_node);

  free(r);
  free(r_index_node);
}

int main(int argc, char *argv[]) {
  double temps;
  int nb_nodes = 0, max_node=0;
  char * in_put = argv[1];
  char * out_put = "out_put/google_scholar.txt";

  nb_nodes = nbEdgesAndNodes(in_put, &max_node);
  nb_nodes ++; // je sais pas pk mais quand je l'enleve ça fait malloc(): memory corruption

  clock_t start = clock();
  // exo1(in_put,out_put, nb_nodes, max_node);

  exo3(in_put, max_node-1);

  temps = (double)(clock()-start)/(double)CLOCKS_PER_SEC;
  printf("\nRun terminée en %.10f seconde(s)!\n", temps);
  return 0;
}
