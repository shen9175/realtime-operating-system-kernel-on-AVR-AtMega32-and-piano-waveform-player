#include "PQ.h"
int Parent(int i)
{
	return (i-1)/2;
}

int Left(int i)
{
	return 2*(i+1)-1;
}

int Right(int i)
{
	return 2*(i+1);
}

void max_heapify(TCB* A, int i)
{
	int l,r,largest;
	TCB temp;
	l=Left(i);
	r=Right(i);
	if(l<PQsize && A[l].Priority>A[i].Priority)
		largest=l;
	else if(l<PQsize && A[l].Priority==A[i].Priority)
		{
			if(A[l].order<A[i].order)
				largest=l;
			else
				largest=i;
		}
	else
		largest=i;  //A[l].Priority<A[i].Priority

	if(r<PQsize && A[r].Priority>A[largest].Priority)
		largest=r;
	else if(r<PQsize && A[r].Priority==A[largest].Priority)
		{
			if(A[r].order<A[largest].order)
				largest=r;			
		}
	else;

	if(largest!=i)
		{
		temp=A[largest];
		A[largest]=A[i];
		A[i]=temp;
		max_heapify(A,largest);
		}
	
}

TCB dePQ(TCB* A)
{
	if(PQsize<0)
		;//printf("heap underflow!\n");
	else
	   {
		TCB max;
		max=A[0];
		A[0]=A[PQsize-1];
		PQsize--;
		max_heapify(A,0);
		return max;
	   }
		
}

void heap_increase_key (TCB *A, int i, TCB key)
{
	  	A[i]=key;
		while(i>0 && A[Parent(i)].Priority<A[i].Priority)
			{
			    TCB temp=A[i];
			    A[i]=A[Parent(i)];
			    A[Parent(i)]=temp;
			    i=Parent(i);
			}
}

void enPQ(TCB *A, TCB item)
{
	PQsize++;
	item.order=PQsize;  //record the order of push in.
	//A[PQsize-1].Priority=-30000;
	heap_increase_key(A,PQsize-1,item);
}
