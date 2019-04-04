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

/* Please set here the value of the highest number
 * representing a node in the graph
 */
#define NUMBER_OF_NODES 200000000

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
void BFS(adjarray* adj){
/*
  fifo = createFifo();
  fifo.add(s);
  Mark(s);
  while fifo
*/
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
