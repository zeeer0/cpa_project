# ifndef __FIFO_H__
# define  __FIFO_H__

typedef struct NodeFifo{
  unsigned int index_n;
  struct NodeFifo * prec;
}NodeFifo;

typedef struct fifo{
  NodeFifo * first;
  NodeFifo * last;
}fifo;

fifo * create_fifo();
void add(fifo * FIFO, unsigned s);
unsigned pop(fifo * FIFO);
int is_empty(fifo * FIFO);

#endif
