# ifndef __EXOS_H__
# define  __EXOS_H__


typedef struct NodeFifo NodeFifo;
struct NodeFifo{
  unsigned int index_n;
  NodeFifo * prec;
};

typedef struct fifo{
  NodeFifo * first;
  NodeFifo * last;
}fifo;

fifo * create_fifo();
void add(fifo * FIFO, unsigned s);
unsigned pop(fifo * FIFO);
int is_empty(fifo * FIFO);

#endif
