#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;

int thread_count;
long long number_of_point;

long long *works, *seeds;
void set_thread_count();
void *CalPi(void* n);
double Rand(long long *seed);

int main(int argc, char* argv[]){
	set_thread_count();
	
	if (argc<2){
		puts("please enter the number of point");
		return 0;
	}
	number_of_point = strtol(argv[1],NULL,10);
//	thread_count = strtol(argv[2],NULL,10);

	pthread_t* thread_handles = (pthread_t*)malloc(thread_count*sizeof(pthread_t));
	works = (long long*)malloc(thread_count*sizeof(long long));
	seeds = (long long*)malloc(thread_count*sizeof(long long));

	for (long long thread=0; thread<thread_count; thread++){
		works[thread] = number_of_point / thread_count;
		seeds[thread] = rand()+1;
	}
	works[0] += number_of_point%thread_count;

	for (long long thread=0; thread<thread_count; thread++){
		pthread_create(&thread_handles[thread], NULL, CalPi, (void*)thread);
	}
	
	long long sum = 0;
	for (long long thread=0; thread<thread_count; thread++){
		void *x;
		pthread_join(thread_handles[thread], &x);
		sum += (long long) x;
	}
	printf("%.10f\n", 4.0*(double)sum/number_of_point);

	free(thread_handles);
	return 0;
}


void set_thread_count(){
	thread_count = thread::hardware_concurrency();
}

void* CalPi(void* thread){
	long long work = works[ (long long)thread ];
	long long seed = seeds[ (long long)thread ];
	long long sum=0;
	while (work--){
		double x = Rand(&seed);
		double y = Rand(&seed);
		if (x*x+y*y<=1.0)sum++;
	}
	return (void*)sum;
}

double Rand(long long *seed){
	*seed = ( (*seed)*0xdefaced+1 )&INT_MAX;
	return (double)(*seed)/INT_MAX;
}
