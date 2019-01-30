#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include "defs.h"

float euclidean_distance(float *x, float *y){
	int i;
	float dst = 0;
	for(i=0;i<M;i++){
		dst += pow(x[i]-y[i],2);
	}
	return sqrt(dst);
}


void vectorSub(float *in1, float *in2, float *out){
	int i;
	for(i=0; i<M; i++){
		out[i] = in1[i] - in2[i];
	}
}

void vectorMultiplyScalar(float *in, float v, float *out){
	int i;
	for(i=0; i<M; i++){
		out[i] = in[i] * v;
	}
}

int toroidal_distance(int x, int y){
	return abs(abs(x - y) - N/2);
}	


