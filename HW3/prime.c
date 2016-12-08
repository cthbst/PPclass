#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define MASTER 0

typedef struct {
	long long max_prime;
	long long cnt;
} data;

int isprimeL(long long n){
	int i, squareroot;
	if (n>10){
		squareroot = (int) sqrt(n);
		for (i=3; i<=squareroot; i+=2){
			if (n%i==0){
				return 0;
			}
		}
		return 1;
	}
	else 
		return 0;
}
int isprime(int n){
	int i, squareroot;
	if (n>10){
		squareroot = (int) sqrt(n);
		for (i=3; i<=squareroot; i+=2){
			if (n%i==0){
				return 0;
			}
		}
		return 1;
	}
	else 
		return 0;
}

int main(int argc, char *argv[]){
	long long pc;
	long long foundone;
	long long n, limit;
	int rank, size;

	MPI_Init(&argc,&argv);

	data d;
	MPI_Datatype datatype, oldtype[1];
	int blockcount[1];
	MPI_Aint offsets[1];
	MPI_Status status;
	
	
	sscanf(argv[1],"%lld",&limit);
	
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	blockcount[0]=2;
	offsets[0]=0;
	oldtype[0]=MPI_LONG_LONG_INT;
	MPI_Type_create_struct(1,blockcount, offsets, oldtype, &datatype);
	MPI_Type_commit(&datatype);

	if (rank==MASTER){
		printf("Starting. Numbers to be scanned= %lld\n",limit);
	}
	
	d.cnt=0;
	d.max_prime=0;
	
	int delta = 2*(size+1); 
	int pos = rank;
	int size2 = size*2;
	int flag;
	for (n=11+rank*2; ; n+=delta, ++pos){
		if (pos==size)pos=0, n-=size2;
		if (n>limit)break;

		flag = n<2147483647LL ? isprime(n) : isprimeL(n);
		if (flag){
			++d.cnt;
			d.max_prime=n;
		}
	}
		
	if (rank!=MASTER){
		MPI_Send(&d,1,datatype,0,0,MPI_COMM_WORLD);
	}
	else {
		data recv;
		int core;
		for (core=1; core<size; core++){
			MPI_Recv(&recv,1,datatype,core,0,MPI_COMM_WORLD,&status);
			d.cnt+=recv.cnt;
			if (recv.max_prime>d.max_prime)
				d.max_prime = recv.max_prime;
		}
	}
	
	MPI_Type_free(&datatype);
	MPI_Finalize();

	if (rank==MASTER){
		printf("Done. Largest prime is %lld Total primes %lld\n",d.max_prime, d.cnt+4);
	}

	return 0;
}
