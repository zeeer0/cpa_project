
/** main.c **/
adjarray* adjArray = readAndStoreInAdjArray(cleanDataFile);
free(adjArray->cd);
free(adjArray->adj);
free(adjArray);
/************/


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

/* la fonction */
adjarray* readAndStoreInAdjArray(char* src){
  adjarray* adjArray = (adjarray*) malloc(sizeof(adjarray));
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

adjArray->adj = (unsigned*) malloc(sizeof(unsigned)*max);
int isPlain[max];
for(long long unsigned i =0 ; i < max; i++){
  adjArray->adj[i] = 0;
  isPlain[i]=0;
}

  nb1=0; nb2=0;
  stream = fopen(src, "r");
  if(stream==NULL){
    printf("No such file\n");
    exit(1);
  }

    // jump commentaries
    line = (char*) malloc(sizeof(char)*1024);
    len=0; nread=0;
    while((nread=getline(&line, &len, stream)) > 0){
     if(line[0]!='#' && line[0]!='\n'){
        fseek(stream, -nread, SEEK_CUR);
        break;
      }
    }
    free(line);

    // rename
    while(fscanf(stream, "%d\t%d", &nb1, &nb2) > 0){
      int array[2] = { nb1, nb2 };
      for(int i=0; i<2 ; i++){
        for(unsigned j=adjArray->cd[rename[array[i]]]; j < adjArray->cd[rename[array[i]]+1];j++){
          if(isPlain[j] == 0){
            if(i==0)
              *(adjArray->adj+j)=rename[array[1]];
            else
              *(adjArray->adj+j)=rename[array[0]];

            isPlain[j]=1;
            break;
          }
        }
      }
    }

  fclose(stream);
free(rename);
  free(degrees);
  printf("Exercice 7 : sucessfully stored the graph in Adjacency Array structure. \n");
  return adjArray;
}
