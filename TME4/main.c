#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define SIZE 400
#define NUMBER_OF_NODES 1000000

// exo 2
void labelPropagation(char* path){
  char* cleanDataFile = cleanData(path);
  int** matrix = readAndStoreInAdjMatrix(cleanDataFile);
  int** arr= nbEdgesAndNodes(cleanDataFile, 0);
  unsigned nb = *(arr[0]);
  int hasChanged = 1, r=0;

// 1. labels = 1 2 3 4 5 ... 400
int* labels = (int*) malloc(sizeof(int)*nb);
for(int i=0;i< nb; i++){
 labels[i]=i;
}

// while highest frenquency neighbords change
while(hasChange){
   // 2. To shuffle an array a of n elements (indices 0..n-1):
   int j=0; node_t* tmp;
   for(int i=0; i < nb-1; i++){
      while( (j = rand()) >= nb || i > j);
      tmp=tab[i];
      tab[i]=tab[j];
      tab[j]=tmp;
   }

  hasChanged = 0;
   // 3-4. test if it change
   for(int i=0; i< nb i++){
    if( (r = highestFrequencyNeighbors(labels, tab[i])) != labels[i] ){
     labels[i] = r;
     hasChanged=1;
    }
  }
}
free(tab);
free(labels);
free(matrix);
free(arr);
}

int main(int argc, char** args){
  if(argc < 1){
    printf("usage : ./tme4.exe proba, with proba between 0.0 and 1.0\n");
    return 1;
  }
  float p = atof(args[1]);
  if(p < 0.0 || p > 1.0){
    printf("usage : ./tme4.exe proba, with proba between 0.0 and 1.0\n");
    return 1;
  }

  srand(time(NULL));
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
 return EXIT_SUCCESS;
}
