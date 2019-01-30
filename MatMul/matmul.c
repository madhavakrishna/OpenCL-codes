#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#include "def.h"

#define MAX_SOURCE_SIZE (0x100000)

void checkErr(cl_int err, const char *name);
void checkKernelEnqueue(cl_int err);
void showDeviceInfo(cl_device_id device_id);

void initMatrix(float *A, int N, int M){
	int i = 0;
	int j = 0;
	for(i = 0; i < N; i = i+1){
		for(j = 0; j < M; j = j+1){
			A[i*M+j] = 10 * (float) rand()/RAND_MAX + 1;	
		}
	}
}


int main()
{
	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_program program = NULL;
	cl_kernel kernel = NULL;

	/* My machine got 2 platforms
 	 * 1. clover (AMD's OpenCl) with AMD GPU as device
 	 * 2. Intel OpenCL with CPU as device
 	 * Note that a single platform can support multiple-devices.
 	 * In my case each platform supports only one device, depends on your system configuration.
 	 * run clinfo (linux) command to get platforms supported in your machine.	
 	 */
	cl_platform_id platform_id[2]; 
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;
     
	FILE *fp;
	char fileName[] = "./mmul.cl";
	char *source_str;
	size_t source_size;
     
	/* Load the source code containing the kernel*/
	fp = fopen(fileName, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel file.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);
     
	/* Get Platform and Device Info */
	ret = clGetPlatformIDs(2, platform_id, &ret_num_platforms);
	checkErr(ret,"platform_id");
	printf("No. of platforms detected: %d\n",ret_num_platforms);
	for(int i=0; i<ret_num_platforms; i++){
		//Experience the portability of OpenCL, choose either CPU or GPU as the device.
		//ret = clGetDeviceIDs(platform_id[i], CL_DEVICE_TYPE_CPU, 1, &device_id, &ret_num_devices);
		ret = clGetDeviceIDs(platform_id[i], CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);
		if(ret == CL_SUCCESS){
			break;
		}
	}
	checkErr(ret,"device_id");
	showDeviceInfo(device_id);

	/* Create OpenCL context */
	context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	checkErr(ret,"Context"); 
	/* Create Command Queue */
	//cl_queue_properties proprt[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
	//command_queue = clCreateCommandQueueWithProperties(context, device_id, proprt, &ret);
	command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &ret);
	checkErr(ret,"Command Queue"); 
     
	/* Create Kernel Program from the source */
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
	checkErr(ret,"Program"); 
     
	/* Build Kernel Program */
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	if(ret != CL_SUCCESS){
		size_t len;
		char   buffer[2048];
		printf("Error: Failed to build program executable!\n");
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		printf("%s\n",buffer);
		exit(EXIT_FAILURE);
	}    
 
	/* Create OpenCL Kernel */
	kernel = clCreateKernel(program, "mmul", &ret);

	float *host_A; //A matrix
	float *host_B; //B matrix
	float *host_C; //B matrix
	int Mdim, Ndim, Pdim; //A[N][P], B[P][M], C[N][M]
	int szA, szB, szC; //number elements

	Ndim = NDIM; 
	Pdim = PDIM;
	Mdim = MDIM;
    
	szA = Ndim*Pdim;
	szB = Pdim*Mdim;
	szC = Ndim*Mdim;
	
	host_A = (float *)malloc(szA*sizeof(float));
	host_B = (float *)malloc(szB*sizeof(float));
	host_C = (float *)malloc(szC*sizeof(float));	

	initMatrix(host_A, Ndim, Pdim);
	initMatrix(host_B, Pdim, Mdim);
	initMatrix(host_C, Ndim, Mdim);

	//----------------- device buffers --------------------
	cl_mem dev_A; //memory object for A matrix 	
	dev_A = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*szA, NULL, NULL);	
	cl_mem dev_B; //memory object for B matrix 		
	dev_B = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(float)*szB, NULL, NULL);
	cl_mem dev_C; //memory object for C matrix 		
	dev_C = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float)*szC, NULL, NULL); 
 
	/* Set OpenCL Kernel Parameters */
	ret = clSetKernelArg(kernel, 0, sizeof(int), &Ndim);
	ret = clSetKernelArg(kernel, 1, sizeof(int), &Mdim);
	ret = clSetKernelArg(kernel, 2, sizeof(int), &Pdim);

	ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), &dev_A); checkErr(ret,"Kernel arg 3");
	ret = clSetKernelArg(kernel, 4, sizeof(cl_mem), &dev_B); checkErr(ret,"Kernel arg 4");
    	ret = clSetKernelArg(kernel, 5, sizeof(cl_mem), &dev_C); checkErr(ret,"Kernel arg 5");
    	ret = clSetKernelArg(kernel, 6, sizeof(float)*Pdim, NULL); checkErr(ret,"Kernel arg 6");


	//------- Write A and B matrices into device memory ----
	ret = clEnqueueWriteBuffer(command_queue, dev_A, CL_TRUE, 0, sizeof(float)*szA, host_A, 0, NULL, NULL); checkErr(ret,"Write Buffer A");	
	ret = clEnqueueWriteBuffer(command_queue, dev_B, CL_TRUE, 0, sizeof(float)*szB, host_B, 0, NULL, NULL);	checkErr(ret,"Write Buffer B");	

	size_t 	global[1]; //global domain size
	size_t	local[1];  //local domain size
    	size_t  global_offset[1];
    	cl_uint nd = 1;
	global[0] = (size_t) Ndim;
	local[0]  = (size_t) (NDIM/10);
    	global_offset[0]=0;
	cl_event event;

    	ret = clEnqueueNDRangeKernel(command_queue, kernel, nd, global_offset, global, local, 0, NULL, &event);
    	checkKernelEnqueue(ret);

	/* Wait for the event object to complete */   
	/* cl_int clWaitForEvents(cl_uint num_events, const cl_event *event_list) */
	clWaitForEvents(1, &event);

	cl_ulong time_start;
 	cl_ulong time_end;
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
	clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);

	double nanoSeconds = time_end-time_start;
 	printf("OpenCL Execution time: %f ms\n",nanoSeconds/1000000.0);
 
	/* Copy results from the memory buffer */
 	ret = clEnqueueReadBuffer(command_queue, dev_C, CL_TRUE, 0, szC * sizeof(float), host_C, 0, NULL, NULL);

	//////////////////// Verify Output ///////////////////////////
	float *C;
	C = (float *)malloc(szC*sizeof(float));	
	//---- Reference Output -----------
	int i,j,k;
	for(i=0; i<Ndim; i++){
		for(j=0; j<Mdim; j++){
			float tmp = 0.0;
			for(k=0; k<Pdim; k++){
				tmp += host_A[i*Pdim+k] * host_B[k*Mdim+j];
			}
			C[i*Mdim+j] = tmp;	
		}
	}

	double tolerance = 0.1;
  	printf("Checking results for correctness ...\n");
  	for (i = 0; i < Ndim; i++) {
    		for (j = 0; j < Mdim; j++) {
      			if (fabs(C[i * Mdim + j] - host_C[i * Mdim + j]) > tolerance) {
        			printf("ERROR MISMATCH in array C i %d j %d CPU SAYS %f and GPU SAYS %f \n", i, j, C[i * Mdim + j], host_C[i * Mdim + j]);
        			exit(EXIT_FAILURE);
      			}
    		}
  	}
  	printf("ALL OK WE ARE DONE \n");
	/////////////////////////////////////////////////////////////////////////////////////////
	
  	free(C);
  	free(host_C);
  	free(host_A);
  	free(host_B);
 	free(source_str);
 
    	/* Finalization */
    	ret = clFlush(command_queue);
    	ret = clFinish(command_queue);
    	ret = clReleaseKernel(kernel);
    	ret = clReleaseProgram(program);
    	ret = clReleaseMemObject(dev_A);
    	ret = clReleaseMemObject(dev_B);
    	ret = clReleaseMemObject(dev_C);
    	ret = clReleaseCommandQueue(command_queue);
    	ret = clReleaseContext(context);
	return 0;
}
