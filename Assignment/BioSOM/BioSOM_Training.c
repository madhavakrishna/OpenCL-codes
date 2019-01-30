#include<stdio.h>
#include<math.h>

#include "defs.h"
#include "funcs.h"
 
extern float W[N][M];
extern float IS[S][M];
extern float beta;

void updateWeights(float *neuron, float *ipvec, int dist){
	float tmp[M];
	vectorSub(neuron, ipvec, tmp);
	float neighValue = beta/pow(2,dist);	
	vectorMultiplyScalar(tmp, neighValue, tmp);
	vectorSub(neuron, tmp, neuron);	
}

void training(){
	int iterCnt;
	int ipIndex;
	int nIndex;

	for(iterCnt=0; iterCnt < MAXITER; iterCnt++){
		for(ipIndex=0; ipIndex < S; ipIndex++){
			float *ipvec = IS[ipIndex];
			int winnerIndex = 0;
			float min_dist  = INFINITY;
			//Find winner neuron
			for(nIndex=0; nIndex < N; nIndex++){
				float *neuron = W[nIndex];	
				float eDist = euclidean_distance(neuron,ipvec);
				if(min_dist > eDist){
					min_dist = eDist;
					winnerIndex = nIndex;
				}	
			}
			//Update Weights
			for(nIndex=0; nIndex < N; nIndex++){
				float *neuron = W[nIndex];	
				int tDist = toroidal_distance(nIndex,winnerIndex);
				updateWeights(neuron, ipvec, tDist);			
			}
		}
		beta = fminf(beta*DECAY, MIN_BETA);
	}
}

void training_loopFusion(){
	int iterCnt;
	int ipIndex;
	int nIndex;

	for(iterCnt=0; iterCnt < MAXITER; iterCnt++){
		float *ipvec, *next_ipvec;
		int winnerIndex, nextWinnerIndex;
		float min_dist = INFINITY;
		
		// ipIndex:0
		winnerIndex = 0;
		ipvec = IS[0];
		for(nIndex=0; nIndex < N; nIndex++){
			float *neuron = W[nIndex];
			float eDist = euclidean_distance(neuron,ipvec);
			if(min_dist > eDist){
				min_dist = eDist;
				winnerIndex = nIndex;
			}
		}

		for(ipIndex=0; ipIndex < (S-1); ipIndex++){
			nextWinnerIndex = 0;
			next_ipvec = IS[ipIndex+1];
			min_dist = INFINITY;
			for(nIndex=0; nIndex < N; nIndex++){
				//update weights
				float *neuron = W[nIndex];
				int tDist = toroidal_distance(nIndex, winnerIndex);
				updateWeights(neuron, ipvec, tDist);
				//Find next winner neuron
				float eDist = euclidean_distance(neuron, next_ipvec);
				if(min_dist > eDist){
					min_dist = eDist;
					nextWinnerIndex = nIndex;
				}
			}
			winnerIndex = nextWinnerIndex;
			ipvec = next_ipvec;
		}

		//update weights based on last input vector
		for(nIndex=0; nIndex < N; nIndex++){
			float *neuron = W[nIndex];
			int tDist = toroidal_distance(nIndex, winnerIndex);
			updateWeights(neuron, ipvec, tDist);
		}
			
		beta = fminf(beta*DECAY, MIN_BETA);
	}
}

