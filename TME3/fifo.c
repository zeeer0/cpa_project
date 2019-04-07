#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "fifo.h"

fifo * create_fifo(){
  fifo * FIFO = (fifo *)malloc(sizeof(struct fifo *));
  FIFO->first = NULL;
  FIFO->last = NULL;
  return FIFO;
}

void add(fifo * FIFO, unsigned int s){
  NodeFifo * nodefifo = (NodeFifo *)malloc(sizeof(struct NodeFifo *));
  nodefifo->index_n = s;
  if(FIFO->last == NULL){
    FIFO->last = nodefifo;
  }
  else{
    FIFO->first->prec = nodefifo;
  }
  nodefifo->prec = NULL;
  FIFO->first = nodefifo;
}

unsigned pop(fifo * FIFO){
  if(FIFO->last == NULL){
    printf("FIFO empty\n");
    exit(0);
  }
  unsigned res = FIFO->last->index_n;
  FIFO->last = FIFO->last->prec;
  return res;
}

int is_empty(fifo * f){
  return f->last == NULL;
}
