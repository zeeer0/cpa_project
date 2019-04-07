#ifndef CLE_H_
#define CLE_H_

typedef struct {
    unsigned int node;
    unsigned int degree;
    unsigned int emplacement;
    int core;
    int * neighbors;
    unsigned int nb_neighbors;
} key;

key * parse_key(int node, int degree);

int eg(key* a, key* b);

int inf(key* a, key* b);

void print(key * c);

#endif /* CLE_H_ */
