#ifndef PQ_H
#define PQ_H

#include"UIK.h"

extern int PQsize;

int Parent(int i);

int Left(int i);

int Right(int i);

void max_heapify(TCB* A, int i);

TCB dePQ(TCB* A);

void heap_increase_key (TCB *A, int i, TCB key);

void enPQ(TCB *A, TCB item);

#endif

