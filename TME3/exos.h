# ifndef __EXOS_H__
# define  __EXOS_H__

typedef struct edgeList{
  int n1;
  int n2;
  struct edgeList* next;
} edgeList;

typedef struct{
  unsigned n; // number of nodes
  unsigned m; // number of edges
  unsigned *cd; // cumulative degree cd[0]=0 length=n+1
  unsigned *adj; // concatened lists of neighbors
} adjarray;

typedef struct Node{
  unsigned id_node;
  struct Node *next;
}Node;

int** nbEdgesAndNodes(char* path, int display);
char* cleanData(char* path);
int* nodeDegree(char*);
void specialQuantity(int* degrees, char*);
void degreeDistribution(int* degrees);
edgeList* readAndStoreInListEdge(char*);
int** readAndStoreInAdjMatrix(char*);
adjarray* readAndStoreInAdjArray(char*);

Node *add_node(int neighbour, Node *adj_nodes);
int** nb_edges_nodes(char* path, int display, unsigned * max_node);
Node **adjacency_array(FILE *f_in, int *rename_node, unsigned nb_nodes);
unsigned bfs(Node** G, unsigned *s, unsigned nb_nodes);
unsigned max_connections(Node** G, unsigned *s, unsigned nb_nodes);
unsigned diameter(Node** G, unsigned nb_nodes);
void max_connections_diameter(char * src);

void numberOfTriangle(adjarray* g);
long long unsigned intersect(unsigned * U, int sizeU, unsigned* V, int sizeV);
void swap(unsigned *a, unsigned *b);
#endif
