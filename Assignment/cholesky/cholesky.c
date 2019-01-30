#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
void show_matrix(float *A, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++){
            printf("%09.4f ", A[i * n + j]);
	}	
        printf("\n");
    }
}

void zeroInitMatrix(float *A, int n){
	for(int i = 0; i < n; i++){
		for(int j = 0; j < n; j++){
			A[i*n+j] = 0;
		}
	}
}

void initInputMatrix(float *A, int n){
	//Positive definite symmetric matrix
	int i, j, k;
	float sum;
	srand(123);

	float *a = (float*)calloc(n*n, sizeof(float));
	float *at = (float*)calloc(n*n, sizeof(float));
		
	for(i= 0; i < n; i++){
        	for(j= 0; j < (i+1) ; j++){
			a[i*n+j] = (rand() % n) + 1.5;
		}
	}

	for(i = 0; i < n; i++) {
		for(j = 0; j < (i+1); j++) {
			at[j*n+i] = a[i*n+j];
		}
	}

	for(i = 0; i < n; i++){ //row of first matrix
		for(j = 0; j < n; j++){  //column of second matrix
    			sum=0;
         		for(k = 0; k < n; k++){
        		    sum = sum + a[i*n+k] * at[k*n+j];
			}
         		A[i*n+j] = sum;
		    }
    	}
	free(a);
	free(at);
}

void cholesky(float *A, float *L, int n) {
	for (int i = 0; i < n; i++){
		for (int j = 0; j < (i+1); j++) {
			float s = 0;
			for (int k = 0; k < j; k++){
				s += L[i * n + k] * L[j * n + k];
			}
			L[i * n + j] = (i == j) ?
				sqrt(A[i * n + i] - s) :
				((1.0 / L[j * n + j]) * (A[i * n + j] - s));
		}
 	}
}

#define N 32
int main() {
	float m[N][N];
	float n[N][N];
			
    	initInputMatrix(&(m[0][0]),N);
    	zeroInitMatrix(&(n[0][0]), N);

	cholesky(&(m[0][0]), &(n[0][0]), N);
    	show_matrix(&(n[0][0]), N);
 	return 0;
}
