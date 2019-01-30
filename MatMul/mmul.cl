
__kernel void mmul( const int Mdim, const int Ndim, const int Pdim, 
		__global float* A, __global float* B, __global float* C, 
		__local float* Bwrk)
{

	int k,j;
	int i = get_global_id(0);
	
	int iloc = get_local_id(0);
	int nloc = get_local_size(0); 
    	int nRd = Pdim/nloc;
       
	float tmp;
	if( (i < Ndim) ){

		for(j=0; j < Mdim; j++){
			for(k=iloc*nRd;k<(iloc+1)*nRd;k++){
				Bwrk[k] = B[k*Mdim+j];
			}
			barrier(CLK_LOCAL_MEM_FENCE); //read-after-write for Bwrk
			tmp = 0.0;
			for(k=0;k<Pdim;k++){
				tmp += A[i*Pdim+k] * Bwrk[k];
			}
			C[i*Mdim+j] = tmp;
			barrier(CLK_LOCAL_MEM_FENCE); //write-after-read for Bwrk
		}
	}		
}
