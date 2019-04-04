#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include "utils.h"

/* Please set here the value of the highest number
 * representing a node in the graph
 */
#define NUMBER_OF_NODES 20000000

void getOutDegreeOfNodes(char* path, int* renommage, int* dout, int* nb)
{
  FILE* stream = fopen(path, "r");
  int nb1, nb2, i;

  // do not forget to use a clean file(without commentaries) otherwise it will not work
    while(fscanf(stream, "%d\t%d", &nb1, &nb2) > 0){
      int array[2] = { nb1, nb2 };
      for(i=0; i< 2; i++){
        if(renommage[array[i]]==0){ // pas de renommage
          renommage[array[i]]=*nb;
          (*nb)++;
         }

         dout[renommage[array[i]]]+=1;
     }

  }
  fclose(stream);

  *nb-=1;
  printf("Nombre de noeuds : %d\n", *nb);
}

void powerIter(char * path, double* P, int * dout, double alpha, int* renommage, int* nb){
  double total = 0.0;
  int i = 0;

  double * vTemp = (double*) malloc(sizeof(double)*(*nb));
  double doubleNb = (double) (*nb);

  for(i =0; i < (*nb); i++)
    vTemp[i]=0.0;

  FILE* stream = fopen(path, "r");
  int nb1, nb2;

  // do not forget to use a clean file(without commentaries) otherwise it will not work
  while(fscanf(stream, "%d\t%d", &nb1, &nb2) > 0){
    vTemp[renommage[nb2]]+= P[renommage[nb1]] * (1.0 / ((double) dout[renommage[nb1]]) );
  }

  fclose(stream);

// calculating total to normalize
for(i=0; i<(*nb); i++){
    total+=vTemp[i];
}

printf("Total before normalize : %.20f\n", total);

// normalize
  if(total < 1.0){
    double toDistrub = (1.0 - total) / doubleNb;
    for(i =0; i < (*nb); i++)
        vTemp[i]+=toDistrub;
  } else if(total > 1.0){
    printf("Erreur de calcul : total > 1\n");
    exit(1);
  }

  // calculation of P(t+1)
  for(i =0; i< (*nb); i++)
    P[i] = ((1.0-alpha) * vTemp[i]) + (alpha * (1.0/doubleNb));

  free(vTemp);
}

void printNumBestAndLowest(char* pathToNames, double* Pt, int* nb, int* renommage, int num){
  double* best = (double*) malloc(sizeof(double)*num);
  int* bestIndex = (int*) malloc(sizeof(int)*num);
  double* lowest = (double*) malloc(sizeof(double)*num);
  int* lowIndex = (int*) malloc(sizeof(int)*num);
  double val, tmp1 = -1.0, tmp2 = -1.0;
  int itmp1=-1, itmp2=-1, number=0,value=0, isValSet=0;
  char *name="", *line = (char*) malloc(sizeof(char)*256), read;
  for(int i=0; i < num; i++){
    best[i] = -1.0;
    lowest[i] =  2.0;
  }

// best
  for(int i =0 ; i < (*nb); i++){
    val = Pt[i];
    for(int j = 0;j < num; j++){
      if(val > best[j]){
        if(j==num-1 || best[j] == -1.0){
           best[j]=val;
           bestIndex[j]=i;
        }else{
          tmp1 = best[j];
          itmp1 = bestIndex[j];
          best[j] = val;
          bestIndex[j]= i;
          for(int h = j+1; h < num; h++){
            if(tmp1!=-1.0){
              tmp2=best[h];
              itmp2 = bestIndex[h];
              best[h]=tmp1;
              bestIndex[h]=itmp1;
              tmp1=-1.0;
            }else{
              tmp1=best[h];
              itmp1 = bestIndex[h];
              best[h]=tmp2;
              bestIndex[h] = itmp2;

            }
          }
        }
        break;
      }
    }
  }

tmp1 = -1.0;
tmp2 = -1.0;
itmp1=-1;
itmp2=-1;
//lowest
  for(int i =0 ; i < (*nb); i++){
    val = Pt[i];
    for(int j = 0;j < num; j++){
      if(val < lowest[j]){
        if(j==num-1 || lowest[j] == 2.0){
           lowest[j]=val;
           lowIndex[j]=i;
        }else{
          tmp1 = lowest[j];
          itmp1 = lowIndex[j];
          lowest[j] = val;
          lowIndex[j]= i;
          for(int h = j+1; h < num; h++){
            if(tmp1!=-1.0){
              tmp2=lowest[h];
              itmp2 = lowIndex[h];
              lowest[h]=tmp1;
              lowIndex[h]=itmp1;
              tmp1=-1.0;
            }else{
              tmp1=lowest[h];
              itmp1 = lowIndex[h];
              lowest[h]=tmp2;
              lowIndex[h] = itmp2;
            }
          }
        }
        break;
      }
    }
  }

for(int i=0; i < num; i++){
    for(int j=0; j< NUMBER_OF_NODES; j++){
      if(renommage[j] == bestIndex[i]){
        bestIndex[i]=j;
        break;
      }
    }
    for(int j=0; j< NUMBER_OF_NODES; j++){
      if(renommage[j] == lowIndex[i]){
        lowIndex[i]=j;
        printf("low index %d : %d\n",i, j);
        break;
      }
    }
}

FILE* stream = fopen(pathToNames, "r");

while((read=(char) fgetc(stream)) != EOF){
  if(read == '\n'){
    line[number]='\0';
    number=0;
    // skip commentaries
    if(line[0]=='#')
      continue;
    isValSet=0;
    for(char* s = line; *s; s++){
      if(isValSet==0 && *s=='\t'){
        *s='\0';
        value=atoi(line);
        name = s+1;
        break;
      }
    }

    // real function
    for(int i =0; i < num; i++){
      if(value == bestIndex[i]){
        printf("best n %d : %s\n", i+1, name);
      }
      if(value == lowIndex[i]){
        printf("low n %d : %s\n", i+1, name);
      }
    }

  }else{
    line[number]=read;
    number++;
  }
}

  fclose(stream);
  free(line);
  free(best);
  free(lowest);
  free(bestIndex);
  free(lowIndex);
}

int main(int argc, char** args){
  if(argc <= 2){
    printf("usage : tme5.exe path/to/file/graph.txt path/to/file/namesOfNodes.txt\n");
    exit(1);
  }

/****** init *********/
int* nb = (int*) malloc(sizeof(int));
int* renommage = (int*) malloc(sizeof(int)*NUMBER_OF_NODES);
int* dout = (int*) malloc(sizeof(int)*NUMBER_OF_NODES);
for(int h =0; h<NUMBER_OF_NODES;h++){
 dout[h]=0;
 renommage[h]=0;
}
*nb=1;

// first read : out degre of each node and renaming
getOutDegreeOfNodes(args[1], renommage, dout, nb);

// create P and init with 1/number of nodes
double * P = (double*) malloc(sizeof(double)*(*nb));
for(int i =0; i < (*nb); i++)
  P[i]=1.0/((double)*nb);

int ITERATION = 10;
double alpha = 0.15;
// second read : calculate P(t+1) ITERATION time
for(int i=0; i< ITERATION; i++){
  printf("Power Iteration n° %d\n", i+1);
  powerIter(args[1], P, dout, alpha, renommage, nb);
}

int NUM = 5;
// print NUM best and lowest pageranks
printNumBestAndLowest(args[2], P, nb, renommage, NUM);

free(P);
free(renommage);
free(dout);
free(nb);
return EXIT_SUCCESS;
}
