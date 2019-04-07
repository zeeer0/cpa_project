#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "utils.h"

// PROTOTYPES
void labelPropagation(adjarray* a, int nbNodes, int* nbOfCommunities, int nbOfCommunitiesIndex, int createTxt);

int main(int argc, char** args){
 if(argc < 3){
   printf("usage : ./tme4.exe path/to/graph.txt numberOfExecutionForLabelPropagation\n");
   return 1;
 }

srand(time(NULL));

// get a clean file
char* cleanDataFile = cleanData(args[1]);

// renaming tab
int * rename = (int*) malloc(sizeof(int)* NUMBER_OF_NODES);
for(int i=0;i<NUMBER_OF_NODES;i++){
  rename[i]=-1;
}

// rename all nodes and count the number of nodes
FILE* stream = fopen(cleanDataFile, "r");
int nb1, nb2, namer =0;
while(fscanf(stream, "%d\t%d", &nb1, &nb2) > 0){
    int array[2] = { nb1, nb2 };
    for(int i=0; i< 2; i++){
      if(rename[array[i]]==-1){ // pas de renommage
        rename[array[i]]=namer;
        namer++;
       }
   }

}
fclose(stream);

int nbNodes = namer;
printf("Number of nodes : %d\n", nbNodes);

// store the graph in adjacency array
adjarray* a = readAndStoreInAdjArray(cleanDataFile);
// tab for counting communities (1000 executions exercise)
int * nbOfCommunities = (int*) malloc(sizeof(int)*1000);
memset(nbOfCommunities, 0, sizeof(int)*1000);

int NUMBER_OF_EXECUTION = atoi(args[2]);
int isGeneratingTxt = 0;

for(int i=0; i < NUMBER_OF_EXECUTION; i++){
  printf("labelPropagation n ° %d\n", i+1);
  if(NUMBER_OF_EXECUTION==1)
    isGeneratingTxt=1;
  // last argument is generating a text for one execution
  // should be set to 1 only if there is one execution
  labelPropagation(a, nbNodes, nbOfCommunities, i, isGeneratingTxt);
}

// generate the text for exercise with 1000 iterations
if(NUMBER_OF_EXECUTION==1000){
  stream = fopen("communities_x1000.txt", "w+");

  for(int i=0; i < nbNodes;i++){
      fprintf(stream, "%d %d\n", i+1, nbOfCommunities[i]);
  }

  fclose(stream);
}

free(a->adj);
free(a->cd);
free(a);
free(rename);
free(nbOfCommunities);
 return EXIT_SUCCESS;
}

/* Exercice 2
 * a : adjacency array
 * nbNodes : number of nodes in the graph
 * nbOfCommunities : tab containing the number of communities for each execution
 * nbOfCommunitiesIndex : the index of current execution
 * createTxt : generating communities_x1.txt
               set to 1 if there is only one execution of labelPropagation else 0
 */
void labelPropagation(adjarray* a, int nbNodes, int* nbOfCommunities, int nbOfCommunitiesIndex, int createTxt){
  int hasChanged = 1, highestLabelNb=0, highestLabelIndex = 0, tmp =0, tmpIndex=0, j=0;
  int* labelOccurence = (int*) malloc(sizeof(int)*nbNodes);
  int* labels = (int*) malloc(sizeof(int)*nbNodes);
  int* nodes = (int*) malloc(sizeof(int)*nbNodes);
  int* communities = (int*) malloc(sizeof(int)*nbNodes);

// 1. init variables and set a unique label for each node
for(int i=0; i < nbNodes; i++){
 labels[i]=i;
 nodes[i]=i;
 labelOccurence[i]=0;
 communities[i]=0;
}

// while highest frenquency neighbords for a node change
while(hasChanged){
   // 2. To shuffle an array a of n elements (indices 0..n-1):
   for(int i=0; i < nbNodes-1; i++){
      while( (j = rand()%nbNodes) < i );
      tmp=nodes[i];
      nodes[i]=nodes[j];
      nodes[j]=tmp;
   }

  hasChanged = 0;
   // 3-4. test if it change over all nodes
   for(int i=0; i< nbNodes; i++){
     //reset
     for(unsigned j = a->cd[nodes[i]]; j < a->cd[nodes[i]+1]; j++){
       if( labelOccurence[labels[a->adj[j]]] !=0)
          labelOccurence[labels[a->adj[j]]]=0;
     }
     //3. count highest label occuring among his neighbours
     highestLabelNb=0;
     highestLabelIndex=0;
     for(unsigned j = a->cd[nodes[i]]; j < a->cd[nodes[i]+1]; j++){
       tmpIndex = labels[a->adj[j]];
       tmp = labelOccurence[tmpIndex]+1;
       if(tmp > highestLabelNb){
         highestLabelNb=tmp;
         highestLabelIndex = tmpIndex;
       }
       labelOccurence[tmpIndex]= tmp;
     }

     // is it different from his label
    if( highestLabelIndex != labels[nodes[i]] ){
     labels[nodes[i]] = highestLabelIndex;
     hasChanged=1; // => go to step 2.
    }
  }
}

// count the number of nodes for each community
for(int i=0; i < nbNodes;i++){
    communities[labels[i]]+=1;
}

// count the number of communities for this execution
for(int i=0; i<nbNodes;i++){
  if(communities[i] > 0){
    nbOfCommunities[nbOfCommunitiesIndex]++;
  }
}

if(createTxt){
  printf("Building communities_x1.txt...\n");
  FILE* stream = fopen("communities_x1.txt", "w+");

  for(int i=0; i < nbNodes;i++){
    if(communities[i] > 0)
      fprintf(stream, "%d %d\n", i+1, communities[i]);
  }

  fclose(stream);
}

printf("Number of communities for execution n ° %d : %d\n", nbOfCommunitiesIndex+1, nbOfCommunities[nbOfCommunitiesIndex]);

free(labelOccurence);
free(labels);
free(nodes);
free(communities);
}
