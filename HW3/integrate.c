#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define PI 3.1415926535897932
#define MASTER 0

int main(int argc, char **argv){
	long long i, num_intervals;
	double rect_width, area, sum, x_middle;

	int rank, size;
	MPI_Status status;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	sscanf(argv[1],"%lld",&num_intervals);

	rect_width = PI / num_intervals;

	sum = 0.0;
	for (i=rank+1; i < num_intervals+1; i+=size){
		x_middle = (i-0.5) * rect_width;
		area = sin(x_middle) * rect_width;
		sum = sum + area;
	}

	if (rank!=MASTER){
		MPI_Send(&sum,1,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
	}
	else {
		double recv;
		int core;
		for (core=1; core<size; core++){
			MPI_Recv(&recv, 1, MPI_DOUBLE, core, 0 ,MPI_COMM_WORLD, &status);
			sum += recv;
		}
	}

	MPI_Finalize();

	if (rank==MASTER){
		printf("The total area is: %.10f\n", (float)sum);
	}

	return 0;
}
