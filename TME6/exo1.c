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

/* Please set here the value of the highest number
 * representing a node in the graph
 */
#define NUMBER_OF_NODES 100000000

#define IGNORE_COMMENTS char is_comment;        \
  sscanf(line, "%c", &is_comment);              \
  if(is_comment == '#'){                        \
    continue;                                   \
  }                                             \

#define MIN(a, b)((((float)a) < ((float)b)) ? ((float)a) : ((float)b))
#define MAX(a, b)((((float)a) > ((float)b)) ? ((float)a) : ((float)b))

typedef struct {
    unsigned int index_node;
    float value;
} node;

/**
* Function to count the number of nodes and edges in file
*   and change the value of max_node
*
* in_put : path of file inPut
* max_node : the max number of the nodes
**/
int** nbEdgesAndNodes(char* path, int display, int * max_node){
   // to count number of nodes
   int* tab = (int*) malloc(sizeof(int)* NUMBER_OF_NODES);
   for(int i =0; i< NUMBER_OF_NODES;i++)
    tab[i]=0;

  int** numbers = (int**) malloc(sizeof(int*)*2);
  int* nbNodes = (int*) malloc(sizeof(int));
  int* nbEdges = (int*) malloc(sizeof(int));
  numbers[0]=nbNodes; numbers[1] = nbEdges;
  *nbNodes = 0;
  *nbEdges = 0;
  long long unsigned nb1=0, nb2=0;
  FILE* stream = fopen(path, "r");
  if(stream==NULL){
    printf("No such file\n");
    exit(1);
  }

  // jump commentaries
  char* line = (char*) malloc(sizeof(char)*1024);
  size_t len=0, nread=0;
  while((nread=getline(&line, &len, stream)) > 0){
   if(line[0]!='#' && line[0]!='\n'){
      fseek(stream, -nread, SEEK_CUR);
      break;
    }
  }
  free(line);

  // count
  while(fscanf(stream, "%llu\t%llu", &nb1, &nb2) > 0){
    long long unsigned array[2] = { nb1, nb2 };
    for(int i =0; i<2; i++){
      if(tab[array[i]]==0){
       tab[array[i]]=1;
       (*nbNodes)++;
       }

       if(((unsigned)*max_node) < array[i]){
         *max_node=array[i];
       }
    }
    (*nbEdges)++;
  }

  fclose(stream);
  free(tab);
  if(display){
   printf("Exercice 2 :\nFile name : %s\nNumber of nodes : %d\nNumber of edges : %d\n", path, *nbNodes, *nbEdges);
  }
   return numbers;
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

    int array[2] = { i, j};
    int h =0;
    for(h=0; h< 2; h++){
      if(index_tab[array[h]]==-1){ /*the node i does not exist in the heap*/
        // add the node in heap
        ajout(heap, parse_key(array[h], 1), index_tab);
        // save the node location in the index table
        index_tab[array[h]]=index_node;
        // increment the index for nodes
  			index_node++;
        // add node j as neighbors of node i
        if(h == 0){
          heap->a[index_tab[array[h]]]->neighbors[heap->a[index_tab[array[h]]]->degree -1] = array[h+1];
        }else{
          heap->a[index_tab[array[h]]]->neighbors[heap->a[index_tab[array[h]]]->degree -1] = array[h-1];
        }
        heap->a[index_tab[array[h]]]->nb_neighbors =1;
  		}else{ /*the node i exist in the heap*/
          // increment the degree of i
          heap->a[index_tab[array[h]]]->degree ++;
          // reallocate the list of neighbors and add node j as neighbors of i
          heap->a[index_tab[array[h]]]->neighbors = realloc(heap->a[index_tab[array[h]]]->neighbors, heap->a[index_tab[array[h]]]->degree*sizeof(int));
          if(h == 0)
            heap->a[index_tab[array[h]]]->neighbors[heap->a[index_tab[array[h]]]->degree -1] = array[h+1];
          else
            heap->a[index_tab[array[h]]]->neighbors[heap->a[index_tab[array[h]]]->degree -1] = array[h-1];

          heap->a[index_tab[array[h]]]->nb_neighbors ++;
  		}
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
    fprintf (f_out, "%d \t %d\n", v->nb_neighbors, v->core);
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
    // degree_density[N[u]] = 0;
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
    tmp_max = ((float)degree_density[u])/((float)u);
    if(max_density<tmp_max){
      max_density = tmp_max;
      index_max_density = (2.0*((float)degree_density[u]))/((float)(u*(u-1)));
      val_max_density = u;
    }
  }
  printf("Average degree densit=%f\nEdge density=%f\nSize of a densest core ordering prefix=%d\n", max_density, index_max_density, val_max_density);
  return 1;
}


int average_degree_density_exo4(char * in_put, node** node_val, int nb_nodes){
  float tmp_max=0.0;
  float max_density = 0.0;
  float index_max_density = 0.0;
  int val_max_density=0;

  int *degree_density = (int*)malloc(nb_nodes*sizeof(int));

  int u=0;
  for(u=0; u<=nb_nodes; u++){
    degree_density[u] = 0;
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
    if(node_val[i]->value > node_val[j]->value){
      degree_density[node_val[i]->index_node]++;
    }else{
      degree_density[node_val[j]->index_node]++;
    }
  }

  for(u=1; u<nb_nodes;u++){
    degree_density[u] = degree_density[u] + degree_density[u-1];
    tmp_max = ((float)degree_density[u])/((float)u);
    if(max_density<tmp_max){
      max_density = tmp_max;
      index_max_density = (2.0*((float)degree_density[u]))/((float)(u*(u-1)));
      val_max_density = u;
    }
  }
  printf("Average degree densit=%f\nEdge density=%f\nSize of a densest core ordering prefix=%d\n", max_density, index_max_density, val_max_density);
  return 1;
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
int MKSCORE(char * in_put, int nb_iterations, node** node_val, int max_node){
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

      int array[2] = {i,j};
      int h=0;
      for(h=0; h<2; h++){
        if (node_val[array[h]]==NULL){
          node_val[array[h]] = (node*)malloc(sizeof(node));
          node_val[array[h]]-> index_node = array[h];
          node_val[array[h]]-> value = 0.0;
        }
      }
      if(node_val[i]->value <= node_val[j]->value){
        node_val[i]->value++;
      }else{
        node_val[j]->value++;
      }
    }
  }
  for(i=0; i<=max_node; i++){
    if(node_val[i]==NULL)
      continue;
    node_val[i]->value= ((float)node_val[i]->value)/((float)t);
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
  int *index_tab = (int*)malloc(sizeof(int)*max_node); // index_tab : the index for each node in the heap
  int *N = (int*)malloc(sizeof(int)*max_node); // list to save the delete order for each node in a heap

  // initialization of both lists index_tab and N to -1
  for(i = 0; i < max_node; i++){
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
void exo4(char * in_put, int max_node){
  int j=0, h=0;  // variable to be used in the for

  node ** node_val = (node**)malloc(sizeof(node*)*(max_node+1));

  // initialization of r_index_node and r to -1
  for(j=0; j<=max_node; j++){
    node_val[j] = NULL;
  }

  MKSCORE(in_put, NB_ITERATIONS, node_val, max_node);
  node* max = (node*)malloc(sizeof(node));
  for(j=0;j<=max_node; j++){
    max=node_val[j];
    for(h=j;h<=max_node; h++){
      if(node_val[h]==NULL)
        continue;
      if((max==NULL && node_val[h]!=NULL) || max->value < node_val[h]->value){
        max= node_val[h];
        max->index_node = h;
      }
    }
    node_val[j]=max;
  }

  // compute the average degree density, the edge density
  // and the size of a densest core ordering prefix
  average_degree_density_exo4(in_put, node_val, max_node);

  /*for(j=0; j<=max_node; j++)
    if(node_val[j]!=NULL)
      free(node_val[j]);
  free(node_val);*/
}

int main(int argc, char *argv[]) {
  double temps;
  int nb_nodes = 0;
  int* max_node=(int*) malloc(sizeof(int));
  *max_node=0;
  char * in_put = argv[1];
  char * out_put = "out_put/google_scholar.txt";

  nb_nodes = *(nbEdgesAndNodes(in_put, 1, max_node)[0]);

  printf("nb_node=%d\n",nb_nodes);
  printf("max_node=%d\n",(*max_node)+1);

  clock_t start = clock();
  exo1(in_put,out_put, nb_nodes+1, (*max_node)+1);

  // exo4(in_put, *max_node);

  temps = (double)(clock()-start)/(double)CLOCKS_PER_SEC;
  printf("\nRun terminée en %.10f seconde(s)!\n", temps);
  return 0;
}
