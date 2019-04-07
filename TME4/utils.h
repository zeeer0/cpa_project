# ifndef __UTILS_H__
# define  __UTILS_H__

#define NUMBER_OF_NODES 2000000

typedef struct{
  unsigned n; // number of nodes
  unsigned m; // number of edges
  unsigned *cd; // cumulative degree cd[0]=0 length=n+1
  unsigned *adj; // concatened lists of neighbors
} adjarray;

adjarray* readAndStoreInAdjArray(char* src);
char* cleanData(char* path);

#endif
