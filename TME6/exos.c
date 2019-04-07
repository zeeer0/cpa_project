#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "key.h"
#include "heap_array.h"

#define SIZE_OF_LINE 100
#define NB_ITERATIONS 10

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

typedef struct NodDegree *NodDegree;
struct NodDegree{
  unsigned int ident;
  unsigned int degree;
  float score;
  unsigned int *voisins;
};

/**
* Function to count the number of nodes and edges in file
*   and change the value of max_node
*
* in_put : path of file inPut
* max_node : the max number of the nodes
**/
int** nb_edges_nodes(char* path, int display, int * max_node){
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
      if(index_tab[array[h]]==-1){ /*the node i (or j) does not exist in the heap*/
        // add the node in heap and in index_tab to save the node location in the index table
        ajout(heap, parse_key(array[h], 1), index_tab);
        // increment the index for nodes
  			index_node++;
  		}else{ /*the node i (or j) exist in the heap*/
          // increment the degree of i (or j)
          heap->a[index_tab[array[h]]]->degree ++;
          // reallocate the list of neighbors and add node j (or i) as neighbors of i (or j)
          heap->a[index_tab[array[h]]]->neighbors = realloc(heap->a[index_tab[array[h]]]->neighbors, heap->a[index_tab[array[h]]]->degree*sizeof(int));
  		}
      // add node j (or i) as neighbors of node i (or j)
      if(h == 0)
        heap->a[index_tab[array[h]]]->neighbors[heap->a[index_tab[array[h]]]->degree -1] = array[h+1];
      else
        heap->a[index_tab[array[h]]]->neighbors[heap->a[index_tab[array[h]]]->degree -1] = array[h-1];

      heap->a[index_tab[array[h]]]->nb_neighbors ++;
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
int calcul_density1(char * in_put, int *N, int nb_nodes){
  int SizeDensestCore=0;
  float tmp_max=0.0, AverageDegreeDensity = 0.0,EdgeDensity = 0.0;
  int *degree_density = (int*)malloc(sizeof(int)*(nb_nodes));

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
    if(N[i]>=N[j]){
      degree_density[N[i]]++;
    }else{
      degree_density[N[j]]++;
    }
  }

  for(u=1; u<nb_nodes;u++){
    degree_density[u] = degree_density[u] + degree_density[u-1];
    tmp_max = ((float)degree_density[u])/((float)u);
    if(AverageDegreeDensity<=tmp_max){
      AverageDegreeDensity = tmp_max;
      EdgeDensity = (2.0*((float)degree_density[u]))/((float)(u*(u-1)));
      SizeDensestCore = u;
    }
  }
  printf("EXO1\nAverage degree densit=%f\nEdge density=%f\nSize of a densest core ordering prefix=%d\n", AverageDegreeDensity, EdgeDensity, SizeDensestCore);
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
  calcul_density1(in_put, N, nb_nodes);
  free(N);
}

// exo3
NodDegree *create_adjacency_tab(FILE *f, unsigned int *rename_tab, unsigned int nb_nodes){

  NodDegree *nodes = (NodDegree*)malloc(sizeof(NodDegree)*nb_nodes);
  unsigned *nb_neighbors_added= (unsigned *)malloc(sizeof(unsigned)*nb_nodes);

  unsigned i, j;
  char line[SIZE_OF_LINE];

  for(i=0; i < nb_nodes; i++){
    nodes[i] = (NodDegree)malloc(sizeof(struct NodDegree));
    nodes[i]->degree=0;
    nodes[i]->ident = i;
    nodes[i]->score = 0.0;
    nb_neighbors_added[i] = 0;
  }
  fseek(f, 0, SEEK_SET);
  while (fgets(line, SIZE_OF_LINE, f) != NULL) {
    IGNORE_COMMENTS;
    sscanf(line, "%u %u", & i, & j);
    i = rename_tab[i];
    nodes[i]->degree++;

    j = rename_tab[j];
    nodes[j]->degree++;
  }

  //allocating nod neighbors
  for(i=0; i < nb_nodes; i++){
    nodes[i]->voisins = (unsigned int*)malloc(nodes[i]->degree*sizeof(unsigned int));
  }

  fseek(f, 0, SEEK_SET);
  while (fgets(line, SIZE_OF_LINE, f) != NULL) {
    IGNORE_COMMENTS;
    sscanf(line, "%u %u", & i, & j);
    i = rename_tab[i];
    j = rename_tab[j];

    nodes[i]->voisins[nb_neighbors_added[i]] = j;
    nb_neighbors_added[i]++;

    nodes[j]->voisins[nb_neighbors_added[j]] = i;
    nb_neighbors_added[j]++;

  }
  free(nb_neighbors_added);
  return nodes;
}

void MKSCORE_adj(NodDegree* G, int n, int nb_iterations){
  int i=0, l=0, j=0;
  unsigned k = 0;
  for(i=0; i<n; i++){
    G[i]->score = 0.0;
  }

  // loop nb iterations
  for(l=0; l<nb_iterations; l++){
    for(i=0; i<n; i++){
      for(k=0; k<G[i]->degree; k++){
        j = G[i]->voisins[k];
        if(j>i){
          if (G[i]->score <= G[j]->score){
            G[i]->score ++;
          }
        }else{
          G[j]->score ++;
        }
      }
    }
  }

  for(i=0; i<n; i++){
    G[i]->score = G[i]->score/((float)nb_iterations);
  }
}

int cmpfunc (const void * a, const void * b){
  NodDegree f = *((NodDegree *)a);
  NodDegree s = *((NodDegree *)b);
  if (f->score < s->score) return  1;
  if (f->score > s->score) return -1;

  return 0;
}

int calcul_density3(NodDegree * G, int nb_nodes){
  float tmp_max=0.0;
  float AverageDegreeDensity = 0.0;
  float EdgeDensity = 0.0;
  int SizeDensestCore=0;

  int *degree_density = (int*)malloc(nb_nodes*sizeof(int));
  int *index_node = (int*)malloc(nb_nodes*sizeof(int));

  int u=0;
  for(u=0; u<nb_nodes; u++){
    degree_density[u] = 0;
    index_node[G[u]->ident] = u;
  }

  int i=0;
  unsigned j=0;
  int index_i=0, index_j;
  for(i=0;i<nb_nodes;i++){
    index_i = index_node[i];
    for (j = 0; j<G[i]->degree; j++) {
      index_j = index_node[G[index_i]->voisins[j]];
      if(G[index_i]->score > G[index_j]->score){
        degree_density[i]++;
      }else{
        degree_density[G[index_i]->voisins[j]]++;
      }
    }
  }

  for(u=1; u<=nb_nodes;u++){
    degree_density[u] = degree_density[u] + degree_density[u-1];
    tmp_max = ((float)degree_density[u])/((float)u);
    if(AverageDegreeDensity<tmp_max){
      AverageDegreeDensity = tmp_max;
      EdgeDensity = (2.0*((float)degree_density[u]))/((float)(u*(u-1)));
      SizeDensestCore = u;
    }
  }
  printf("Average degree densit=%f\nEdge density=%f\nSize of a densest core ordering prefix=%d\n", AverageDegreeDensity, EdgeDensity, SizeDensestCore);
  return 1;
}

void exo3(char * in_put, unsigned max_node, unsigned nb_nodes, unsigned t){
  unsigned int *rename_tab = (unsigned int*)malloc(sizeof(unsigned int)*max_node);
  unsigned i=0, j=0, cpt=0;
  for(i = 0; i < max_node; i++){
    rename_tab[i] = 0;
  }

  char line[SIZE_OF_LINE];
  FILE * f_in;
  if ((f_in = fopen(in_put, "r")) == NULL) {
    fprintf(stderr, "\nErreur: Impossible de lire le fichier %s\n", in_put);
    return;
  }
  fseek(f_in, 0, SEEK_SET);
  while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
    IGNORE_COMMENTS;
    sscanf(line, "%u %u", & i, & j);
    rename_tab[i] = 1;
    rename_tab[j] = 1;
  }

  for(i = 0; i <max_node; i++){
    if(rename_tab[i] == 1){
      rename_tab[i] = cpt;
      cpt++;
    }
  }

  NodDegree *G = create_adjacency_tab(f_in, rename_tab, nb_nodes);
  MKSCORE_adj(G, nb_nodes, t);
  qsort(G, nb_nodes, sizeof(NodDegree), cmpfunc);
  printf("******exo3******\n");
  unsigned f=0;
  for(f=0; f<nb_nodes; f++){
    printf("G[%u] : node=%u, score=%u\n",f, G[f]->ident, G[f]->score);
  }
  printf("****fin exo3****\n");
  // calcul_density3(G, nb_nodes);
}
// exo 3

// EXO4
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
  }

  for(t=0; t<nb_iterations; t++){
    fseek(f_in, 0, SEEK_SET);
    while (fgets(line, SIZE_OF_LINE, f_in) != NULL) {
      IGNORE_COMMENTS;
      sscanf(line, "%d %d", & i, & j);

      if (node_val[i]!=NULL && node_val[j]!=NULL){
        if(node_val[i]->value <= node_val[j]->value){
          node_val[i]->value++;
        }else{
          node_val[j]->value++;
        }
      }
    }
  }

  for(i=0; i<max_node; i++){
    if(node_val[i]==NULL)
      continue;
    node_val[i]->value= ((float)node_val[i]->value)/((float)t);
  }
  return 1;
}

int calcul_density4(char * in_put, node** node_val, int max_node){
  float tmp_max=0.0;
  float AverageDegreeDensity = 0.0;
  float EdgeDensity = 0.0;
  int SizeDensestCore=0;

  int *degree_density = (int*)malloc(max_node*sizeof(int));
  int *index_node = (int*)malloc(max_node*sizeof(int));

  int u=0;
  for(u=0; u<max_node; u++){
    if(node_val[u]==NULL){
      degree_density[u] = NULL;
      continue;
    }
    degree_density[u] = 0;
    index_node[node_val[u]->index_node] = u; // noeud et son ordre dans la struct
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
    if(node_val[index_node[i]]->value > node_val[index_node[j]]->value){
      degree_density[index_node[i]]++;
    }else{
      degree_density[index_node[j]]++;
    }
  }

  for(u=0; u<max_node;u++){
    if(u>0){
      degree_density[u] = degree_density[u] + degree_density[u-1];
    }
    tmp_max = ((float)degree_density[u])/((float)u+1.0);
    if(AverageDegreeDensity<tmp_max){
      AverageDegreeDensity = tmp_max;
      if(u==0){
        EdgeDensity = (2.0*((float)degree_density[u]))/((float)(u+1.0));
      }else{
        EdgeDensity = (2.0*((float)degree_density[u]))/((float)(u*(u+1.0)));
      }
      SizeDensestCore = u;
    }
  }
  printf("Average degree densit=%f\nEdge density=%f\nSize of a densest core ordering prefix=%d\n", AverageDegreeDensity, EdgeDensity, SizeDensestCore);
  return 1;
}

/**
* Launch of the third exercise
*
* in_put : path of file inPut
* max_node : the max number of the nodes
**/
void exo4(char * in_put, int max_node){
  int j=0, h=0;  // variable to be used in the for

  node ** node_val = (node**)malloc(sizeof(node*)*(max_node));

  // initialization of r_index_node and r to -1
  for(j=0; j<max_node; j++){
    node_val[j] = NULL;
  }

  MKSCORE(in_put, NB_ITERATIONS, node_val, max_node);

  // sort
  int tmp_node;

  node* max = NULL;
  node* tmpmax = NULL;
  for(j=0;j<max_node; j++){
    max=node_val[j];
    tmp_node=j;
      for(h=j;h<max_node; h++){
        if(node_val[h]==NULL)
          continue;
        if((max==NULL && node_val[h]!=NULL) || max->value < node_val[h]->value){
          max = node_val[h];
          tmp_node = h;
        }
      }
      tmpmax = node_val[j];
      node_val[j]=max;
      node_val[tmp_node]=tmpmax;
  }
  // compute the average degree density, the edge density
  // and the size of a densest core ordering prefix
  calcul_density4(in_put, node_val, max_node);

  /*for(j=0; j<=max_node; j++)
    if(node_val[j]!=NULL && node_val[j]!=max)
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

  nb_nodes = *(nb_edges_nodes(in_put, 1, max_node)[0]);

  printf("nb_node=%d\n",nb_nodes);
  printf("max_node=%d\n",(*max_node)+1);

  clock_t start = clock();
  exo1(in_put,out_put, nb_nodes+1, (*max_node)+1);

  // exo3(in_put, (*max_node)+1, nb_nodes+1, NB_ITERATIONS);

  // exo4(in_put, (*max_node)+1);

  temps = (double)(clock()-start)/(double)CLOCKS_PER_SEC;
  printf("\nRun terminée en %.10f seconde(s)!\n", temps);
  return 0;
}
