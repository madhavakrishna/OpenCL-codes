#include<stdio.h>
#include<stdlib.h>

#define ROW 256
#define COL 256

int R=(ROW-2);
int C=(COL-2);

void harris(float* I, float* Y){
	float Ix[ROW][COL];
	float Iy[ROW][COL];

	int x,y;

	for(x=1; x <= R; x++){
		for(y=1; y<= C; y++){
			float tmpx = 0.083333333f * ( -I[(x-1)*COL+y-1] - 2*I[(x-1)*COL+y] - I[(x-1)*COL+y+1] + I[(x+1)*COL+y-1] + 2*I[(x+1)*COL+y] + I[(x+1)*COL+y+1] );

			float tmpy = 0.083333333f * ( -I[(x-1)*COL+y-1] - 2*I[x*COL+y-1] - I[(x+1)*COL+y-1] + I[(x-1)*COL+y+1] + 2*I[x*COL+y+1] + I[(x+1)*COL+y+1] );

			Ix[x][y] = tmpx ;
			Iy[x][y] = tmpy ;
		}
	}


	for(x=2; x<=R-1; x++){
		for(y=2; y<=C-1; y++){

			float Sxx = 0;
			float Syy = 0;
			float Sxy = 0;

			int i,j;
			for(i=-1; i<2 ;i++){
				for(j=-1; j<2; j++){
					float tmpx = Ix[x+i][y+j];
					float tmpy = Iy[x+i][y+j];
					Sxx = Sxx + (tmpx * tmpx);
					Syy = Syy + (tmpy * tmpy);
					Sxy = Sxy + (tmpx * tmpy);
				}
			}

			float trace = Sxx + Syy ;
			float det   = (Sxx * Syy) - (Sxy * Sxy);
			Y[x*COL+y] = det - 0.04f * trace * trace;

		}
	}
}


int main(){

	float I[ROW][COL];
	float Y[ROW][COL];

	srand(12345);
	for(int i=0; i<ROW; i++){
		for(int j=0; j<COL; j++){
			I[i][j] = (rand()%8)*1;
			Y[i][j] = 0;
		}
	}

	harris(&(I[0][0]), &(Y[0][0]));

	return 0;
}
