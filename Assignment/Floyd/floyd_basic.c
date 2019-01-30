#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>

double t_start, t_end;

double rtclock(){
    struct timezone Tzp;
    struct timeval Tp;
    int stat;
    stat = gettimeofday (&Tp, &Tzp);
    if (stat != 0) printf("Error return from gettimeofday: %d",stat);
    return(Tp.tv_sec + Tp.tv_usec*1.0e-6);
}

void init_array(float *f, int n)
{
	int i, j;
	srand(123);
	for(i= 0; i < n; i++) {
		for(j= 0; j < n; j++) {
			int ra = rand() % n;
			if(i == j){
            			f[i*n+j] = (float) ra;
			}
        	}
    	}
}


void print_array(float *f, int n)
{
	int i, j;
	for(i= 0; i < n; i++) {
		for(j= 0; j < n; j++) {
			printf("%f ", f[i*n+j]);
        	}
		printf("\n");
    	}
}

#define min(a,b) ((a)<(b)? a : b)

#define NUM_NODES 1024

int main()
{
	int k, x, y;
	float pathDistanceMatrix[NUM_NODES][NUM_NODES];
	/*
	* pathDistanceMatrix is the adjacency matrix (square) with
	* dimension length equal to number of nodes in the graph
	*/
	init_array(&(pathDistanceMatrix[0][0]),NUM_NODES);

 	t_start = rtclock();

	for(k=0; k < NUM_NODES; k++) {
		for(y=0; y < NUM_NODES; y++) {
			for(x=0; x < NUM_NODES; x++) {
				pathDistanceMatrix[y][x] = min(pathDistanceMatrix[y][k] + pathDistanceMatrix[k][x],pathDistanceMatrix[y][x]);
            		}
        	}
    	}

	t_end = rtclock();
	fprintf(stderr, "time = %0.6lfs\n", (t_end - t_start));
    	//print_array(&(pathDistanceMatrix[0][0]),n);
    	return 0;
}
