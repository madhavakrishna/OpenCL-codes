#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include<unistd.h>
#include<sys/time.h>

#include "defs.h"
#include "funcs.h"
 
//Weight Matrix for one bacterial genome
float W[N][M];
//Sequence of input vectors
float IS[S][M];

float beta = BETA_INIT;

void initRandomMatrix(float *x, int row, int col){
	int i,j;
	for(i=0; i<row; i++){
		for(j=0; j<col; j++){
			x[i*col+j] = (float) (rand() & MAXVAL);  	
		}
	}
}

double rtclock() {
	struct timezone Tzp;
	struct timeval Tp;
	int stat;
	stat = gettimeofday(&Tp, &Tzp);
	if(stat != 0) printf("Error return from gettimeofday: %d",stat);
	return( Tp.tv_sec + Tp.tv_usec*1.0e-6);
}


void training();

int main(){
	printf("=== Sequential BioSOM ===\n");
	printf("\t Map size (Dimension): %d\n",N);
	printf("\t Sequence length: %d\n",M);
	printf("\t Input size: %d\n", S);
	printf("\t Max. iteration count: %d\n",MAXITER);
	printf("=========================\n");

	srand(1);

	initRandomMatrix( &(W[0][0]),  N, M);
	initRandomMatrix( &(IS[0][0]), S, M);

	double t_start, t_end;
	t_start = rtclock();
	
	training();
	
	t_end = rtclock();
	
	double elapsed = t_end - t_start;
	
	printf("Execution time: %f seconds\n",elapsed);

}
