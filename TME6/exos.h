# ifndef __EXOS_H__
# define  __EXOS_H__

#include "key.h"
#include "heap_array.h"

#define IGNORE_COMMENTS char is_comment;        \
  sscanf(line, "%c", &is_comment);              \
  if(is_comment == '#'){                        \
    continue;                                   \
  }                                             \

#define MIN(a, b)((((float)a) < ((float)b)) ? ((float)a) : ((float)b))
#define MAX(a, b)((((float)a) > ((float)b)) ? ((float)a) : ((float)b))

typedef struct {
    unsigned int index_node;
    float value;
} node;

typedef struct NodDegreeAdjArray{
  unsigned int *neighbors;
  unsigned int degree;
  unsigned int id_node;
  float value;
}NodDegreeAdjArray;

int** nb_edges_nodes(char* path, int display, int * max_node);
tas* create_heap(char * in_put, int*index_tab, int nb_nodes);
int* k_core_decomposition(char* out_put, tas* heap, int * index_tab, int* N);
int calcul_density1(char * in_put, int *N, int nb_nodes);
void k_core_decomposition_calcule(char * in_put, char* out_put, int nb_nodes, int max_node);
int compare (const void * a, const void * b);
void MKSCORE_memory(NodDegreeAdjArray** G, unsigned nb_nodes, unsigned nb_iterations);
void calcul_density_memory(NodDegreeAdjArray ** G, unsigned nb_nodes);

void densest_subgraph(char * in_put, unsigned max_node, unsigned nb_nodes, unsigned t);
int MKSCORE_main_memory(char * in_put, int nb_iterations, node** node_val, int max_node);
int calcul_density_main_memory(char * in_put, node** node_val, int nb_nodes);
void densest_subgraph_main_memory(char * in_put, int max_node);
#endif
