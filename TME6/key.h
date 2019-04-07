#ifndef CLE_H_
#define CLE_H_

typedef struct {
    unsigned int node;
    unsigned int degree;
    unsigned int nb_neighbors;
    int core;
    int * neighbors;
} key;

key * create_key(int node, int degree);

int eg(key* a, key* b);

int inf(key* a, key* b);

void print(key * c);

#endif /* CLE_H_ */
