#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <float.h>
#include <time.h>

/* Please set here the value of the highest number
 * representing a node in the graph
 */
#define NUMBER_OF_NODES 20000000

// PROTOTYPES
void getOutDegreeOfNodes(char* path, int* renommage, int* dout, int* nb);
void getInDegreeOfNodes(char* path, int* renommage, int* din, int* nb);
double powerIter(char * path, double* P, int * dout, double alpha, int* renommage, int* nb);
void printNumBestAndLowest(char* pathToNames, double* Pt, int* nb, int* renommage, int num, double toDistrub);
//

int main(int argc, char** args){
  if(argc < 4){
    printf("usage : ./tme5.exe path/to/file/graph.txt path/to/file/namesOfNodes.txt numberOfIteration\n");
    exit(1);
  }

/****** init *********/
int* nb = (int*) malloc(sizeof(int));
int* nb2 = (int*) malloc(sizeof(int));
int* renommage = (int*) malloc(sizeof(int)*NUMBER_OF_NODES);
int* renommage2 = (int*) malloc(sizeof(int)*NUMBER_OF_NODES);
int* dout = (int*) malloc(sizeof(int)*NUMBER_OF_NODES);
int* din = (int*) malloc(sizeof(int)*NUMBER_OF_NODES);
for(int h =0; h<NUMBER_OF_NODES;h++){
 dout[h]=0;
 din[h]=0;
 renommage[h]=-1;
 renommage2[h]=-1;
}
*nb=0;
*nb2=0;

// 1. first read : out degre of each node and renaming
getOutDegreeOfNodes(args[1], renommage, dout, nb);

// in degrees
getInDegreeOfNodes(args[1], renommage2, din, nb2);

// create P and init with 1/number of nodes
//2. P2 P3 P4 P5 for Correlations exercise
double * P = (double*) malloc(sizeof(double)*(*nb));
double * P2 = (double*) malloc(sizeof(double)*(*nb));
double * P3 = (double*) malloc(sizeof(double)*(*nb));
double * P4 = (double*) malloc(sizeof(double)*(*nb));
double * P5 = (double*) malloc(sizeof(double)*(*nb));
for(int i =0; i < (*nb); i++){
  P[i]=1.0/((double)*nb);
  P2[i]=1.0/((double)*nb);
  P3[i]=1.0/((double)*nb);
  P4[i]=1.0/((double)*nb);
  P5[i]=1.0/((double)*nb);
}

int ITERATION = atoi(args[3]);
double alpha = 0.15;
double toDistrub = 0.0;

// 1. second read : calculate P(t+1) ITERATION time
for(int i=0; i< ITERATION; i++){
  printf("Power Iteration n° %d\n", i+1);
  toDistrub +=powerIter(args[1], P, dout, alpha, renommage, nb);
}

// 2. Correlations exercise
// calculating in-degrees
printf("Building inDegree.txt\n");
FILE* f = fopen("inDegree.txt", "w+");
for(int i=0; i< (*nb); i++){
  fprintf(f, "%.20f %d\n", P[i], din[i]);
}
fclose(f);

// calculating out-degrees
printf("Building outDegree.txt\n");
f = fopen("outDegree.txt", "w+");
for(int i=0; i< (*nb); i++){
  fprintf(f, "%.20f %d\n", P[i], dout[i]);
}
fclose(f);

// 2.3
toDistrub = 0.0;
// calculate P(t+1) ITERATION time
for(int i=0; i< ITERATION; i++){
  printf("Power Iteration 2 n° %d\n", i+1);
  toDistrub +=powerIter(args[1], P2, dout, 0.1, renommage, nb);
}

printf("Building correlationsPart3.txt\n");
f = fopen("correlationsPart3.txt", "w+");
for(int i=0; i< (*nb); i++){
  fprintf(f, "%.20f %.20f\n", P[i], P2[i]);
}
fclose(f);

// 2.4
toDistrub = 0.0;
// calculate P(t+1) ITERATION time
for(int i=0; i< ITERATION; i++){
  printf("Power Iteration 3 n° %d\n", i+1);
  toDistrub +=powerIter(args[1], P3, dout, 0.2, renommage, nb);
}

printf("Building correlationsPart4.txt\n");
f = fopen("correlationsPart4.txt", "w+");
for(int i=0; i< (*nb); i++){
  fprintf(f, "%.20f %.20f\n", P[i], P3[i]);
}
fclose(f);

// 2.5
toDistrub = 0.0;
// calculate P(t+1) ITERATION time
for(int i=0; i< ITERATION; i++){
  printf("Power Iteration 4 n° %d\n", i+1);
  toDistrub +=powerIter(args[1], P4, dout, 0.5, renommage, nb);
}

printf("Building correlationsPart5.txt\n");
f = fopen("correlationsPart5.txt", "w+");
for(int i=0; i< (*nb); i++){
  fprintf(f, "%.20f %.20f\n", P[i], P4[i]);
}
fclose(f);

// 2.6
toDistrub = 0.0;
// calculate P(t+1) ITERATION time
for(int i=0; i< ITERATION; i++){
  printf("Power Iteration 5 n° %d\n", i+1);
  toDistrub +=powerIter(args[1], P5, dout, 0.9, renommage, nb);
}

printf("Building correlationsPart6.txt\n");
f = fopen("correlationsPart6.txt", "w+");
for(int i=0; i< (*nb); i++){
  fprintf(f, "%.20f %.20f\n", P[i], P5[i]);
}
fclose(f);

int NUM = 5;
// 1. print NUM best and lowest pageranks
printNumBestAndLowest(args[2], P, nb, renommage, NUM, toDistrub);

free(P);
free(P2);
free(P3);
free(P4);
free(P5);
free(renommage);
free(renommage2);
free(dout);
free(din);
free(nb);
free(nb2);
return EXIT_SUCCESS;
}

void getOutDegreeOfNodes(char* path, int* renommage, int* dout, int* nb)
{
  FILE* stream = fopen(path, "r");
  int nb1, nb2, i;

  // do not forget to use a clean file(without commentaries) otherwise it will not work
    while(fscanf(stream, "%d\t%d", &nb1, &nb2) > 0){
      int array[2] = { nb1, nb2 };
      for(i=0; i< 2; i++){
        if(renommage[array[i]]==-1){ // pas de renommage
          renommage[array[i]]=*nb;
          (*nb)++;
         }
       }

     dout[renommage[nb1]]+=1;
  }
  fclose(stream);

  printf("Nombre de noeuds : %d\n", *nb);
}

void getInDegreeOfNodes(char* path, int* renommage, int* din, int* nb)
{
  FILE* stream = fopen(path, "r");
  int nb1, nb2, i;

  // do not forget to use a clean file(without commentaries) otherwise it will not work
    while(fscanf(stream, "%d\t%d", &nb1, &nb2) > 0){
      int array[2] = { nb1, nb2 };
      for(i=0; i< 2; i++){
        if(renommage[array[i]]==-1){ // pas de renommage
          renommage[array[i]]=*nb;
          (*nb)++;
         }
       }

     din[renommage[nb2]]+=1;
  }
  fclose(stream);
}

double powerIter(char * path, double* P, int * dout, double alpha, int* renommage, int* nb){
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
double toDistrub;
  if(total < 1.0){
    toDistrub = (1.0 - total) / doubleNb;
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

  return toDistrub;
}

void printNumBestAndLowest(char* pathToNames, double* Pt, int* nb, int* renommage, int num, double toDistrub){
  char * line = (char*) malloc(sizeof(char)* 256);
  char read, *name;
  int isValSet=0, value=0, number=0;
  int isItOk=1;

  int * bestIndex = (int*) malloc(sizeof(int)*num);
  double * bestValues = (double*) malloc(sizeof(double)*num);

  int * lowIndex = (int*) malloc(sizeof(int)*num);
  double * lowValues = (double*) malloc(sizeof(double)*num);

  for(int i=0; i<num;i++){
    lowIndex[i]=-1;
    lowValues[i]=DBL_MAX;

    bestIndex[i]=-1;
    bestValues[i]=0;
  }

  for(int i = 0; i < num; i++){
    for(int h=0; h < (*nb); h++){
      if(Pt[h] > bestValues[i]){
        isItOk=1;
        for(int j=0; j< i; j++){
          if(bestIndex[j]==h){
            isItOk=0;
            break;
          }
        }
        if(isItOk){
          bestIndex[i]=h;
          bestValues[i]=Pt[h];
        }
      }
    }
  }

  for(int i = 0; i < num; i++){
    for(int h=0; h < (*nb); h++){
      if(Pt[h] < lowValues[i]){
        isItOk=1;
        for(int j=0; j< i; j++){
          if(lowIndex[j]==h){
            isItOk=0;
            break;
          }
        }
        if(isItOk && Pt[h]!=toDistrub){
          lowIndex[i]=h;
          lowValues[i]=Pt[h];
        }
      }
    }
  }

// getting real indexes
for(int i=0; i < num; i++){
    for(int j=0; j< NUMBER_OF_NODES; j++){
      if(renommage[j] == bestIndex[i]){
        bestIndex[i]=j;
        break;
      }
    }
  }

// lowers' real indexes this time
  for(int i=0; i < num; i++){
    for(int j=0; j< NUMBER_OF_NODES; j++){
      if(renommage[j] == lowIndex[i]){
        lowIndex[i]=j;
        break;
      }
    }
}

// find names now
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

    /** real function **/
    for(int i =0; i < num; i++){
      if(value == bestIndex[i]){
        printf("Best n %d : %s\n", i+1, name);
      }
      if(value == lowIndex[i]){
        printf("Low n %d : %s\n", i+1, name);
      }
    }
    /********************/

  }else{
    line[number]=read;
    number++;
  }
}

  fclose(stream);
  free(lowValues);
  free(lowIndex);
  free(bestIndex);
  free(bestValues);
  free(line);
}
