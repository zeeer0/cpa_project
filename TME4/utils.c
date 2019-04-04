#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include "utils.h"

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

  return matrix;
}
