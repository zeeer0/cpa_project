#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"

// exo 2
void labelPropagation(int* labelOccurence, int* labels, int* nodes,int * communities, adjarray* a, int nbNodes, int* nbOfCommunities, int nbOfCommunitiesIndex){
  int hasChanged = 1, highestLabelNb=0, highestLabelIndex = 0, tmp =0, tmpIndex=0, j=0;
// 1. labels = 1 2 3 4 5 ... 400
for(int i=0; i < nbNodes; i++){
 labels[i]=i;
 nodes[i]=i;
 labelOccurence[i]=0;
 communities[i]=0;
}

// while highest frenquency neighbords change
while(hasChanged){
   // 2. To shuffle an array a of n elements (indices 0..n-1):
   for(int i=0; i < nbNodes-1; i++){
      while( (j = rand()%nbNodes) < i );
      tmp=nodes[i];
      nodes[i]=nodes[j];
      nodes[j]=tmp;
   }

  hasChanged = 0;
   // 3-4. test if it change
   for(int i=0; i< nbNodes; i++){
     //3. highest label occuring among his neighbours
     for(unsigned j = a->cd[nodes[i]]; j < a->cd[nodes[i]+1]; j++){
       if( labelOccurence[labels[a->adj[j]]] !=0)
          labelOccurence[labels[a->adj[j]]]=0;
     }

     highestLabelNb=0;
     highestLabelIndex=0;
     // calculate occurences
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

for(int i=0; i < nbNodes;i++){
  if(communities[labels[i]] < 1)
    communities[labels[i]]+=1;
}

for(int i=0; i<nbNodes;i++){
  if(communities[i] > 0){
    nbOfCommunities[nbOfCommunitiesIndex]++;
  }
}

}

int main(int argc, char** args){
  /*
  if(argc < 1){
    printf("usage : ./tme4.exe proba, with proba between 0.0 and 1.0\n");
    return 1;
  }
  float p = atof(args[1]);
  if(p < 0.0 || p > 1.0){
    printf("usage : ./tme4.exe proba, with proba between 0.0 and 1.0\n");
    return 1;
  }



  float q;
  while((q=(rand()/(RAND_MAX*1.0)))>p);
  printf("p: %f, q : %f\n",p,q);

  char *s = malloc(sizeof(char)*41);
  s[28]='\0';
  sprintf(s, "./generated/graph_p%f_q%f.txt", p,q);
  int fd = open(s, O_CREAT | O_TRUNC | O_WRONLY);

  int** graph = (int**) malloc(sizeof(int*)*SIZE);
  for(int i=0;i<SIZE;i++)
   graph[i]=(int*) malloc(sizeof(int)*SIZE);

  for(int i=0; i< 400; i++){
    for(int j=i+1;j < 400; j++){
     if( ((int) i/100) == ((int) j/100) ){
        if((rand()/(RAND_MAX*1.0)) <= p){
         dprintf(fd, "%d %d\n",i+1, j+1);
       }
     }else if((rand()/(RAND_MAX*1.0)) <= q){
      dprintf(fd, "%d %d\n",i+1, j+1);
     }
    }
  }

 close(fd);
 printf("%s created.\n",s);
 */

 // exo 2
 if(argc < 1){
   printf("usage : ./tme4.exe path/to/youtube.txt\n");
   return 1;
 }

srand(time(NULL));

char* cleanDataFile = cleanData(args[1]);

int * rename = (int*) malloc(sizeof(int)* NUMBER_OF_NODES);
for(int i=0;i<NUMBER_OF_NODES;i++){
  rename[i]=-1;
}

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

adjarray* a = readAndStoreInAdjArray(cleanDataFile);
int* labelOccurence = (int*) malloc(sizeof(int)*nbNodes);
int* labels = (int*) malloc(sizeof(int)*nbNodes);
int* nodes = (int*) malloc(sizeof(int)*nbNodes);
int * nbOfCommunities = (int*) malloc(sizeof(int)*1000);
int* communities = (int*) malloc(sizeof(int)*nbNodes);

for(int i=0; i < 1000; i++){
  printf("labelPropagation x%d\n", i);
  labelPropagation(labelOccurence, labels, nodes,communities, a, nbNodes, nbOfCommunities, i);
}

printf("Building communities1000x.txt...\n");
stream = fopen("communities1000x.txt", "w+");

for(int i=0; i < 1000;i++){
    fprintf(stream, "%d %d\n", i, nbOfCommunities[i]);
}

fclose(stream);

free(a->adj);
free(a->cd);
free(a);
free(rename);
free(nbOfCommunities);
free(labelOccurence);
free(labels);
free(nodes);
free(communities);
 return EXIT_SUCCESS;
}
