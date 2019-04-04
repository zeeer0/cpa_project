#include <stdio.h>
#include <stdlib.h>
#include "exos.h"

int main(int argc, char** args) {
  if(argc < 2){
   printf("usage : tme3.exe path/graphTxt.txt\n");
   return 1;
  }

// Exercice 2
nbEdgesAndNodes(args[1], 1);
// Exercice 3
char* cleanDataFile = cleanData(args[1]);
// Exercice 4
int* degreeOfNodes = nodeDegree(cleanDataFile);
// Exercice 5
specialQuantity(degreeOfNodes, cleanDataFile);
// Exercice 6
degreeDistribution(degreeOfNodes);
free(degreeOfNodes);
// Exercice 7
//List of Edges
  edgeList* list = readAndStoreInListEdge(cleanDataFile);
  edgeList* cur = list;
  edgeList* next = list->next;
  while(cur->next!=NULL){
    free(cur);
    cur=next;
    next=next->next;
  }
  free(cur);
// Adjacency Matrix
  int** matrix = readAndStoreInAdjMatrix(cleanDataFile);
  free(matrix);
// Adjacency Array
adjarray* adjArray = readAndStoreInAdjArray(args[1]);
// Exercice 8
  //BFS(..);
// Exercice 9
numberOfTriangle(adjArray);
  free(adjArray->cd);
  free(adjArray->adj);
  free(adjArray);
free(cleanDataFile);
 return EXIT_SUCCESS;
}
