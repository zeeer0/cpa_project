#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <sys/wait.h>
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
 printf("File name : %s\nNumber of nodes : %d\nNumber of edges : %d\n", path, *nbNodes, *nbEdges);
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

 printf("Building %s...\n", cpy);
 if(fork()==0){
  if(execl("/bin/bash","bash", "./dataCleaner.sh", path, cpy, NULL)==-1){
   printf("error\n");
  }
 }
 wait(NULL);
 return cpy;
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
  printf("Sucessfully stored the graph in Adjacency Array structure. \n");
  return adjArray;
}
