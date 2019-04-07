#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <math.h>
#include "exos.h"
#include "fifo.h"

/* Please set here the value of the highest number
 * representing a node in the graph
 */
#define NUMBER_OF_NODES 200000000

#define SIZE_LINE 100
#define NB_ITERATIONS 9

#define IGNORE_COMMENTS char is_comment;        \
  sscanf(line, "%c", &is_comment);              \
  if(is_comment == '#'){                        \
    continue;                                   \
  }                                             \

/* Exercice 2
 * path : path to file
 */
int** nbEdgesAndNodes(char* path, int display){
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

/* Exercice 3
 * path : path to file to clean
*/
char* cleanData(char* path){
 char* cpy = (char*) malloc(sizeof(char)*128);
 strncpy(cpy, path, strlen(path)-4);
 strcat(cpy, "-clean.txt");

 printf("Exercice 3 :\nBuilding %s...\n", cpy);
 if(fork()==0){
  if(execl("/bin/bash","bash", "./dataCleaner.sh", path, cpy, NULL)==-1){
   printf("error\n");
  }
 }
 wait(NULL);
 return cpy;
}

/* Exercice 4
* src : path to cleaned file
*/
int* nodeDegree(char* src) {
 int fd = open("nodesDegree.txt", O_WRONLY | O_CREAT | O_TRUNC);

 printf("Exercice 4 :\nBuilding nodesDegree.txt...\n");

 // tab containing degrees of each node, contain -1 if node does not exist
 int* degrees = (int*) malloc(sizeof(int)* NUMBER_OF_NODES);
 for(int i =0; i< NUMBER_OF_NODES;i++)
  degrees[i]=-1;

 int nb1=0, nb2=0;
 FILE* stream = fopen(src, "r");
 if(stream==NULL){
   printf("No such file\n");
   exit(1);
 }

// calculate degree
 while(fscanf(stream, "%d\t%d", &nb1, &nb2) > 0){
   degrees[nb1] = (degrees[nb1]==-1) ? 1 : degrees[nb1]+1;
   degrees[nb2] = (degrees[nb2]==-1) ? 1 : degrees[nb2]+1;
 }

fclose(stream);

 // write all degrees to file
for(int i=0; i < NUMBER_OF_NODES; i++){
  if(degrees[i]!=-1){
    dprintf(fd, "%d %d\n",i, degrees[i]);
  }
}

 close(fd);
 return degrees;
}

/* Exercice 5
 * degrees : tab containing degrees of each nodes from exercice 4
 * src : cleaned file
 */
void specialQuantity(int* degrees, char* src){
 printf("Exercice 5 :\nCalculing special quantity...\n");

 unsigned long long* result = (unsigned long long*) malloc(sizeof(unsigned long long));
 *result = 0;

 // begin running timer
 clock_t begin = clock();

 long long unsigned nb1=0, nb2=0;
 FILE* stream = fopen(src, "r");
 if(stream==NULL){
   printf("No such file\n");
   exit(1);
 }

//calculate
 while(fscanf(stream, "%llu\t%llu", &nb1, &nb2) > 0)
    (*result)+=(degrees[nb1]*degrees[nb2]);

 fclose(stream);

 // end time
 clock_t end = clock();
 double time_spent = (double)(end-begin) / CLOCKS_PER_SEC;

 printf("Special quantity: %llu\nCalculated in %f sec\n", *result, time_spent);
}

/* Exercice 6
 * degrees : tab containing degrees of each nodes from exercice 4
*/

void degreeDistribution(int* degrees){
  printf("Exercice 6 :\nBuilding degreeDistribution.txt...\n");
  // get max degree
  int max = 0;
  for(int i=0; i < NUMBER_OF_NODES; i++){
    if(degrees[i] > max)
      max = degrees[i];
  }

  // degree distributions tab
  int* degreeDistribution = (int*) malloc(sizeof(int)* (max+1));
  for(int i =0; i < max+1; i++)
    degreeDistribution[i] = 0;

  // calculate distribution
  for(int i=0; i < NUMBER_OF_NODES; i++){
    if(degrees[i]!=-1)
      degreeDistribution[degrees[i]]++;
  }

   //write distrubutions to a file
   FILE* stream = fopen("degreeDistribution.txt", "w+");

   for(int i =0; i < max+1; i++){
     if(degreeDistribution[i]!=0)
       fprintf(stream, "%d %d\n", i, degreeDistribution[i]);
   }

  free(degreeDistribution);
  fclose(stream);
}

/* Exercice 7
 * src : cleaned file(without commentaries), otherwise it will not work
*/
edgeList* readAndStoreInListEdge(char* src){
 int nb1=0, nb2=0;
 FILE* stream = fopen(src, "r");
 if(stream==NULL){
   printf("No such file\n");
   exit(1);
 }

 // init edgeList
 edgeList* list = (edgeList*) malloc(sizeof(edgeList));
 edgeList* pointer = list;

 // count
 while(fscanf(stream, "%d\t%d", &nb1, &nb2) > 0){
   pointer->n1 = nb1;
   pointer->n2 = nb2;
   pointer->next = (edgeList*) malloc(sizeof(edgeList));
   pointer=pointer->next;
 }

 fclose(stream);
 // DOES NOT ATTEIGN HERE WITH BIG GRAPHS, OUT OF MEMORY BEFORE
 printf("Exercice 7 : sucessfully stored the graph in List of Edges structure. \n");
 return list;
}

int** readAndStoreInAdjMatrix(char* src){
  // init matrix
  int numberOfNode = *(nbEdgesAndNodes(src, 0)[0]);
  printf("Needed memory : %lu\n", (sizeof(int*)*numberOfNode)*(sizeof(int)*numberOfNode));
  int** matrix = (int**) malloc(sizeof(int*)*numberOfNode);
  for(int i=0; i< numberOfNode; i++){
    matrix[i] = (int*) malloc(sizeof(int)*numberOfNode);
  }
  for(int i=0; i < numberOfNode; i++){
    for(int j=0; j < numberOfNode; j++){
      matrix[i][j]=0;
    }
  }

  // rename all nodes
  int* rename = (int*) malloc(sizeof(int)*NUMBER_OF_NODES);
  for(int i=0;i< NUMBER_OF_NODES; i++)
    rename[i]=-1;

  int nb=0, nb1=0, nb2=0;
  FILE* stream = fopen(src, "r");
  if(stream==NULL){
    printf("No such file\n");
    exit(1);
  }
    // rename
    while(fscanf(stream, "%d\t%d", &nb1, &nb2) > 0){
      if(rename[nb1]==-1){
        rename[nb1]=nb;
        nb++;
      }

      if(rename[nb2]==-1){
        rename[nb2]= nb;
        nb++;
      }
    }

  fclose(stream);

  /******* fill matrix *********/
  nb1=0; nb2=0;
  stream = fopen(src, "r");
  if(stream==NULL){
    printf("No such file\n");
    exit(1);
  }

  // fill
  while(fscanf(stream, "%d\t%d", &nb1, &nb2) > 0){
    matrix[rename[nb1]][rename[nb2]]=1;
    matrix[rename[nb2]][rename[nb1]]=1;
  }

  fclose(stream);

  // NEED 2*numberOfNode*sizeof(int) in MEMORY, BETTER THAN STORING EDGES
  printf("Exercice 7 : sucessfully stored the graph in Adjacency Matrix structure. \n");
  return matrix;
}

adjarray* readAndStoreInAdjArray(char* src){
  adjarray* adjArray = (adjarray*) malloc(sizeof(adjarray));
  adjArray->n=0;
  adjArray->m=0;
  // rename all nodes and make degrees tab
  int* rename = (int*) malloc(sizeof(int)*NUMBER_OF_NODES);
  int* degrees = (int*) malloc(sizeof(int)*NUMBER_OF_NODES);
  for(int i=0;i< NUMBER_OF_NODES; i++){
    rename[i]=-1;
    degrees[i]=-1;
  }

  int nb=0, nb1=0, nb2=0;
  FILE* stream = fopen(src, "r");
  if(stream==NULL){
    printf("No such file\n");
    exit(1);
  }

    // rename
    while(fscanf(stream, "%d\t%d", &nb1, &nb2) > 0){
      int array[2] = { nb1, nb2 };
      for(int i=0; i<2 ; i++){
        if(rename[array[i]]==-1){
          rename[array[i]]=nb;
          nb++;
          adjArray->n++;
        }
        degrees[rename[array[i]]] = (degrees[rename[array[i]]]==-1) ? 1 : degrees[rename[array[i]]]+1;
      }
      adjArray->m++;
    }

  fclose(stream);

  // cd
  adjArray->cd = (unsigned*) malloc(sizeof(unsigned)*(adjArray->n+1));
  adjArray->cd[0] = 0;
  for(unsigned i=0;i < adjArray->n; i++){
    adjArray->cd[i+1]=adjArray->cd[i]+degrees[i];
  }

//adj
long long unsigned max = 0;
for(int i =0 ; i < NUMBER_OF_NODES; i++)
  if(degrees[i]!=-1)
    max+=degrees[i];

free(degrees);
adjArray->adj = (unsigned*) malloc(sizeof(unsigned)*max);
char* isPlain = (char*) malloc(sizeof(char)*max);

for(long long unsigned i =0 ; i < max; i++){
  adjArray->adj[i] = 0;
  isPlain[i]='N';
}

  nb1=0; nb2=0;
  stream = fopen(src, "r");
  if(stream==NULL){
    printf("No such file\n");
    exit(1);
  }

    // fill adj
    while(fscanf(stream, "%d\t%d", &nb1, &nb2) > 0){
      int array[2] = { nb1, nb2 };
      for(int i=0; i<2 ; i++){
        for(unsigned j=adjArray->cd[rename[array[i]]]; j < adjArray->cd[rename[array[i]]+1];j++){
          if(isPlain[j] == 'N'){
            if(i==0)
              *(adjArray->adj+j)=rename[array[1]];
            else
              *(adjArray->adj+j)=rename[array[0]];

            isPlain[j]='Y';
            break;
          }
        }
      }
    }

  fclose(stream);
  free(isPlain);
  free(rename);
  printf("Exercice 7 : sucessfully stored the graph in Adjacency Array structure. \n");
  return adjArray;
}

/* Exercice 8
 * ...
 */
Node *add_node(int neighbour, Node *adj_nodes){
 Node *node = (Node *)malloc(sizeof(Node));
 node->next = adj_nodes;
 node->id_node = neighbour;
 return node;
}

int** nb_edges_nodes(char* path, int display, unsigned * max_node){
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

Node **adjacency_array(FILE *f_in, int *rename_node, unsigned nb_nodes){
 unsigned i, j;
 char line[SIZE_LINE];
 Node **adj_nodes = (Node **)malloc(sizeof(Node*)*nb_nodes);

 // initialization of adjacency_array
 for(i=0; i < nb_nodes; i++){
   adj_nodes[i] = NULL;
 }

 // Store in adjacency_array
 fseek(f_in, 0, SEEK_SET);
 while (fgets(line, SIZE_LINE, f_in) != NULL) {
     IGNORE_COMMENTS
   sscanf(line, "%u %u", & i, & j);
   // rename nodes
   i = rename_node[i];
   j = rename_node[j];

   adj_nodes[i] = add_node(j, adj_nodes[i]);
   adj_nodes[j] = add_node(i, adj_nodes[j]);
 }

 return adj_nodes;
}

unsigned bfs(Node** G, unsigned *s, unsigned nb_nodes){
 unsigned i=0, u=0, v=0;
 fifo * FIFO = create_fifo();

 unsigned *Mark = (unsigned*)malloc(sizeof(unsigned)*nb_nodes);
 unsigned *distances = (unsigned *)malloc(sizeof(unsigned)*nb_nodes);

 for(i=0; i < nb_nodes; i++){
   Mark[i] = 0;
   distances[i] = 0;
 }

 add(FIFO, *s);
 Mark[*s] = 1;

 while(!is_empty(FIFO)){
   u = pop(FIFO);
   // get neighbours of node u
   Node *neighbours = G[u];
   // for each v neighbor of u in G
   while(neighbours){
     v = neighbours->id_node;
     // check if v is not marked
     if(Mark[v] == 0){
       // add node v in FIFO
       add(FIFO, v);
       // marcked v
       Mark[v] = 1;
       distances[v] = distances[u] + 1;
     }
     // move to the next neighbour
     neighbours = neighbours->next;
   }
 }
 // change the node pointer s
 *s=u;
 return distances[u];
}

unsigned max_connections(Node** G, unsigned *s, unsigned nb_nodes){
 fifo * FIFO = create_fifo();
 unsigned i=0, u=0, v=0, max = 0;
 unsigned nb_connections = 0, max_nb_connections = 0, nb_nodes_visited=0;

 unsigned *Mark = (unsigned *)malloc(sizeof(unsigned)*nb_nodes);
 unsigned *distances = (unsigned *)malloc(sizeof(unsigned)*nb_nodes);

 for(i=0; i < nb_nodes; i++){
   Mark[i] = 0;
   distances[i] = 0;
 }

	while(nb_nodes_visited< nb_nodes){
 	nb_connections = 1;
   nb_nodes_visited++;

 	/*for(i=0; i < nb_nodes; i++)
 	   if(Mark[i] == 0)
 		  u = i;
       */
   // bfs algorithm
   add(FIFO, *s);
   Mark[*s] = 1;
   i=0;
   while(!is_empty(FIFO)){
 		u = pop(FIFO);
 		Node *neighbours = G[u];
 		while(neighbours){
       v = neighbours->id_node;
 			if(Mark[v] == 0){
 				add(FIFO, v);
 				Mark[v] = 1;
 				distances[v] = distances[u] + 1;
         // increment the number of connections
         nb_connections++;
         // increment the number of nodes visited
         nb_nodes_visited++;
 			}
   		neighbours = neighbours->next;
   	}
     // find the max the number of connections
    	if (nb_connections > max_nb_connections)
   		max_nb_connections = nb_connections;
   }
	}
 printf("%u",max_nb_connections);
 return max_nb_connections;

}

unsigned diameter(Node** G, unsigned nb_nodes){
 unsigned i=0, j=0, s=0;
 unsigned curr_d = 0, new_d = 0, res = 0;

 while(i<NB_ITERATIONS){
   // choose a node randomly to start
   s = rand() % nb_nodes;
   j=0;
   while(j<NB_ITERATIONS){
     // node s is update by bfs to move to the next node
     new_d = bfs(G, &s, nb_nodes);
     // break is we finds the same diameter
     if(new_d == curr_d)
       break;
     curr_d = new_d;
     j++;
   }
   // save the max diameter
   if(new_d > res)
	    res = new_d;
   i++;
 }
 return res;
}

void max_connections_diameter(char * src) {
 unsigned node_begin=0, max_connection_value = 0, diamater_value=0;
 unsigned i = 0, j=0, nb_nodes=0, nb = 0;
 unsigned* max_node=(int*) malloc(sizeof(int));
 Node **adj_nodes;

 *max_node=0;
 nb_nodes = *(nb_edges_nodes(src, 0, max_node)[0]);

 // list to rename nodes
 unsigned *rename_node = (unsigned *)malloc(sizeof(unsigned)*(*(max_node)+1));
 // to save the degree of each nodes
 unsigned *degrees = (unsigned *)malloc(sizeof(unsigned)*(*(max_node)+1));

 FILE * f_in;
 char line[SIZE_LINE];

 if ((f_in = fopen(src, "r")) == NULL) {
   fprintf(stderr, "\nError: Can not read file %s.\n", src);
   exit(1);
 }

 // rename all nodes
 for(i = 0; i <= *(max_node); i++){
   rename_node[i] = -1;
   degrees[i] = -1;
 }

 while (fgets(line, SIZE_LINE, f_in) != NULL) {
   IGNORE_COMMENTS
   sscanf(line, "%u %u", & i, & j);
   rename_node[i] = 1;
   rename_node[j] = 1;
   // increment the degree of i and j
   degrees[i]++;
   degrees[j]++;
 }

 /*nb is at the end the number of nodes*/
 for(i = 0; i <=*(max_node); i++){
   // if node is present
   if(rename_node[i] == 1)
     rename_node[i] = nb++;
 }

 // the adjacency_array
 adj_nodes = adjacency_array(f_in, rename_node, nb_nodes);

 // find the max connections
 max_connection_value = max_connections(adj_nodes, &node_begin, nb_nodes);
 printf("Max Connection : %d\n", max_connection_value);

	diamater_value = diameter(adj_nodes,nb_nodes);
	printf("diamater_value : %d \n",diamater_value);

 // free adjacency array
 for(i=0; i < nb_nodes; i++){
   Node *node = adj_nodes[i];
   while(node){
     Node *tmp = node->next;
     free(node);
     node = node->next;
   }
 }
 free(adj_nodes);
 free(rename_node);
 fclose(f_in);
}



int cmpfunc (const void * a, const void * b) {
   return ( *(unsigned*)a - *(unsigned*)b );
}

/* Exercice 9
 * g : Adjacency Array representing the graph and build with function from Exercice 7
 */
void numberOfTriangle(adjarray* g){
  long long unsigned total = 0;
  int sizeU=0, sizeV=0, sizeNotToMatch = 0;
  // list to avoid duplicate couples
  unsigned* notToMatch = (unsigned*) malloc(sizeof(unsigned)*g->n);
  for(unsigned i=0; i < g->n; i++){
    notToMatch[i]=0;
  }

  // sort every list of neighbors
  unsigned min = INT_MAX;
  int indice = 0;
  for(unsigned i=0; i < g->n; i++){
    sizeU = g->cd[i+1]-g->cd[i];
    // qsort
     qsort(g->adj+g->cd[i], sizeU, sizeof(unsigned), cmpfunc);
  }

  // begin running timer
  clock_t begin = clock();

  // calculate number of triangle
  for(unsigned i=0; i < g->n; i++){
    for(unsigned* j = g->adj+g->cd[i]; j < g->adj+g->cd[i+1]; j++)
    {
      // to avoid duplicate couples
      if(sizeNotToMatch > 0 && notToMatch[*j]==1){
        continue;
      }

      // (i,*j) is an edge
      {
          sizeU = g->cd[i+1]-g->cd[i];
          sizeV = g->cd[(*j)+1]-g->cd[*j];
          total+= intersect(g->adj+g->cd[i], sizeU, g->adj+g->cd[*j], sizeV);
      }
    }

    notToMatch[i]=1;
    if(sizeNotToMatch==0)
      sizeNotToMatch=1;
  }

  // end time
  clock_t end = clock();
  double time_spent = (double)(end-begin) / CLOCKS_PER_SEC;

  free(notToMatch);
  printf("Exercice 9:\nNumber of triangle in this graph is : %llu\nCalculation time : %f\n", total/3, time_spent);
}

long long unsigned intersect(unsigned * U, int sizeU, unsigned* V, int sizeV){
  long long unsigned numberOfTriangle = 0;
  int pointerToU = 0;
  int pointerToV = 0;

  while(pointerToU < sizeU && pointerToV < sizeV){
    if(U[pointerToU]==V[pointerToV])
    {
      numberOfTriangle++;
      pointerToU++;
      pointerToV++;
    }else if(U[pointerToU] < V[pointerToV]){
        pointerToU++;
    }
    else{
        pointerToV++;
    }
  }

  return numberOfTriangle;
}

// Swapping algorithm
void swap(unsigned *a, unsigned *b)
{
    unsigned temp = *a;
    *a = *b;
    *b = temp;
}
