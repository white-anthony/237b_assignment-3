#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h>
#include <math.h>
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif
#include "readMatrix.h"

float float_abs(float x) {
    if (x >= 0) {
        return x; 
    } 
    return -x; 
}

/**
 * @brief Reads a file's contents into a string and stores its size.
 *        The caller is responsible for freeing *r_source_str.
 *
 * @param filename The name of the file.
 * @param r_source_str The address of the character array to store the string.
 * @param r_source_size The address to store the size of the string.
 *
 * @return SUCCESS if and only if the read is successful.
 */
errcode loadKernelSrc(char* filename, char** r_source_str, size_t* r_source_size) {
  // load kernel source code into array source_str 
  FILE *fp; 
  fp = fopen(filename, "r"); 
  
  // couldn't open the file
  if (NULL == fp) {
    fprintf(stderr, "ERROR: Failed to load kernel file.\n"); 
    return FAILURE; 
  }

  // get the file size
  // go to the end of the file
  fseek(fp, 0, SEEK_END); 
  // the program source string does not need to be null-terminated
  // so location of the end of the file is the same as the size 
  *r_source_size = ftell(fp); 
  // go back to the start of the file for fread
  rewind(fp);
  
  // allocate and read in the file contents
  *r_source_str = (char*)malloc(*r_source_size); 
  if (*r_source_str == NULL) {
    perror("Could not allocate memory for the matrix.\n");
    return FAILURE; 
  }
  fread(*r_source_str, 1, *r_source_size, fp);
  
  // close the file
  fclose(fp); 
  return SUCCESS;
}


int main(int argc, char** argv) {
  float *hostA; // The A matrix
  float *hostB; // The B matrix
  float *hostC; // The output C matrix
  cl_mem deviceA; // The A matrix on the device
  cl_mem deviceB; // The B matrix on the device
  cl_mem deviceC; // The output matrix on the device.
  int numARows;    // number of rows in the matrix A
  int numAColumns; // number of columns in the matrix A
  int numBRows;    // number of rows in the matrix B
  int numBColumns; // number of columns in the matrix B
  // int numCRows;    // number of rows in the matrix C (you have to set this)
  // int numCColumns; // number of columns in the matrix C (you have to set
                   // this)
  

  
  // @./solution -e ./Dataset/0/output.raw -i ./Dataset/0/input0.raw,./Dataset/0/input1.raw -t matrix
  // parse arguments
  // ./solution <A> <B> <C> 

  if (argc != 4) {
    perror("Incorrect number of arguments. Usage is \'./solution <A> <B> <C>\'\n");
    exit(EXIT_FAILURE);
  } 
  
  // file names
  char* fileA = argv[1];
  char* fileB = argv[2];
  // E is the expected matrix
  char* fileE = argv[3];
  
  // read in A, B, and expected matrix
  if (readMatrix(fileA, &hostA, &numARows, &numAColumns) != SUCCESS) {
    perror("Could not read file for matrix A.\n");
    exit(EXIT_FAILURE); 
  }
  printf("Read file for matrix A.\n");
  if (readMatrix(fileB, &hostB, &numBRows, &numBColumns) != SUCCESS) {
    perror("Could not read file for matrix B.\n");
    exit(EXIT_FAILURE); 
  }
  printf("Read file for matrix B.\n");
  // E is the expected matrix
  float* E; 
  int numERows;
  int numEColumns;
  if (readMatrix(fileE, &E, &numERows, &numEColumns) != SUCCESS) {
    perror("Could not read file expected result.\n");
    exit(EXIT_FAILURE); 
  }
  printf("Read file for matrix E.\n");


  printf("Dimensions of A: %d x %d\n", numARows, numAColumns);
  printf("Dimensions of B: %d x %d\n", numBRows, numBColumns);
  

  // read .cl source file
  char* fileSrc = "kernel.cl";
  char* source_str = NULL; 
  size_t source_size = 0;
  if (loadKernelSrc(fileSrc, &source_str, &source_size) != SUCCESS) {
    perror("ERROR: Could not open kernel source file.\n");
    exit(EXIT_FAILURE);
  }
  printf("Read source file.\n");

  // Compute C 
  // Get platform and device information 
  cl_platform_id platform_id = NULL; 
  cl_device_id device_id = NULL; 
  cl_uint ret_num_devices; 
  cl_uint ret_num_platforms;
  // ret stores the return code and can be CL_SUCCESS or any number of error codes
  cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms); 
  ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, 
       &device_id, &ret_num_devices);
  if (ret != CL_SUCCESS) {
    perror("Could not get device IDs.\n"); 
    exit(EXIT_FAILURE); 
  }
  printf("Got platform and device information.\n");

  // create openCL context
  cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
  if (ret != CL_SUCCESS) {
    perror("Could not create an OpenCL context.\n"); 
    exit(EXIT_FAILURE); 
  }
  printf("Got OpenCL context.\n");

  // create command queue 
#ifdef __APPLE__
  // macOS comes with an older version of OpenCL so we use this deprecated function instead
  cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
#else 
  cl_command_queue command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &ret);
#endif

  if (ret != CL_SUCCESS) {
    perror("Could not create command queue.\n"); 
    exit(EXIT_FAILURE); 
  } 
  printf("Created command queue.\n");

  // \begin part that students should definitely do 
  // @@ define size dimensions of result matrix
  const size_t numCRows = numARows; 
  const size_t numCColumns = numBColumns;
  // size of matrices A, B, C in bytes
  const size_t sizeA = numARows * numAColumns * sizeof(float);
  const size_t sizeB = numBRows * numBColumns * sizeof(float);
  const size_t sizeC = numCRows * numCColumns * sizeof(float);


  // @@ create device buffers for A, B, C
  deviceA = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeA, NULL, &ret);
  if (ret != CL_SUCCESS) {
    perror("Could not create A buffer!\n"); 
    fprintf(stderr, "errorcode %d\n", ret);
    exit(EXIT_FAILURE); 
  }
  deviceB = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeB, NULL, &ret);
  if (ret != CL_SUCCESS) {
    perror("Could not create B buffer!\n"); 
    fprintf(stderr, "errorcode %d\n", ret);
    exit(EXIT_FAILURE); 
  }
  deviceC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeC, NULL, &ret);
  if (ret != CL_SUCCESS) {
    perror("Could not create C buffer!\n"); 
    fprintf(stderr, "errorcode %d\n", ret);
    exit(EXIT_FAILURE); 
  }
  printf("Created device buffers.\n");

  // @@ copy host memory into device buffers
  ret = clEnqueueWriteBuffer(command_queue, deviceA, CL_TRUE, 0, 
    sizeA, hostA, 0, NULL, NULL);
  ret = clEnqueueWriteBuffer(command_queue, deviceB, CL_TRUE, 0, 
    sizeB, hostB, 0, NULL, NULL);
  if (ret != CL_SUCCESS) {
    perror("Could not write to device buffers!\n"); 
    exit(EXIT_FAILURE); 
  }
  printf("Buffers written.\n");
  
  // create a program from the kernel source
  cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source_str, 
      (const size_t *)&source_size, &ret);
  if (ret != CL_SUCCESS) {
    perror("Could not create program!\n"); 
    exit(EXIT_FAILURE); 
  }

  free(source_str);

  // @@ build the program 
  ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL); 
  // while other error checks are out of an abundance of caution, 
  // this one should definiitely be included
  if (ret != CL_SUCCESS) {
    perror("ERROR: Could not build the kernel program.\n");
    // get the size of the error log
    size_t log_size = 0; 
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

    // get the error log
    char* log = (char*)malloc(log_size);
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
    // print the error log
    fprintf(stderr, "%s\n", log);

    exit(EXIT_FAILURE);
  }
  printf("Kernel program built.\n");


  // create the kernel 
  cl_kernel kernel = clCreateKernel(program, "matrix_multiply", &ret);

  /*
   * Kernel signature:
  __kernel void matrix_multiply(__global const float *A, __global const float *B, __global float *C
      __global const numARows, __global const numAColumns, 
      __global const numBRows, __global const numBColumns, 
      __global const numCRows, __global const numCColumns)
   */
  // @@ set arguments for the kernel 
  ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&deviceA);
  ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&deviceB);
  ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&deviceC);
  ret = clSetKernelArg(kernel, 3, sizeof(int), (void*)&numARows);
  ret = clSetKernelArg(kernel, 4, sizeof(int), (void*)&numAColumns);
  ret = clSetKernelArg(kernel, 5, sizeof(int), (void*)&numBRows);
  ret = clSetKernelArg(kernel, 6, sizeof(int), (void*)&numBColumns);
  ret = clSetKernelArg(kernel, 7, sizeof(int), (void*)&numCRows);
  ret = clSetKernelArg(kernel, 8, sizeof(int), (void*)&numCColumns);
  
  
  
  // @@ define local and global work sizes
  size_t tilesize = 16;
  size_t local_work_size[] = {tilesize, tilesize};
  size_t global_work_size[] = {
    ((numCRows + local_work_size[0] - 1)/local_work_size[0]) * local_work_size[0], 
    ((numCColumns + local_work_size[1] - 1)/local_work_size[1]) * local_work_size[1]
    }; 
  // @@ run the kernel
  ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, 
      global_work_size, local_work_size, 0, NULL, NULL);
  if (ret != CL_SUCCESS) {
    perror("ERROR: could not enqueue kernel.\n"); 
    exit(EXIT_FAILURE);
  } 
  printf("kernel executed.\n");

  //@@ allocate host matrix C
  hostC = (float*)malloc(sizeC); 
  if (NULL == hostC) {
    perror("Could not allocate memory for hostC.\n");
    exit(EXIT_FAILURE);
  }
  
  //@@ read device C to matrix C
  ret = clEnqueueReadBuffer(command_queue, deviceC, CL_TRUE, 0, sizeC, hostC, 0, NULL, NULL);
  if (ret != CL_SUCCESS) {
    perror("ERROR: could not copy C to host.\n");
    exit(EXIT_FAILURE);
  }
  printf("copied product from device to host\n");

  // \end part that students do
  // check result
  
  if (numCColumns != numEColumns || numCRows != numERows) {
    printf("sizes are not the same.\n");
  }
  


  bool same = true;
  for (int i = 0; i < numERows; ++i) {
    for (int j = 0; j < numEColumns; ++j){ 
      float Eval = E[i * numEColumns + j];
      float Cval = hostC[i * numEColumns + j];
      if (float_abs(Eval - Cval) > 0.01) {
        printf("Matrices are not the same. row: %d, col: %d, C: %f, E: %f \n", 
          i, j, Cval, Eval);
        same = false;
      }
    }
  }

  if (same == true) {
    printf("Matrices are the same!\n");
  }


  ret = clFlush(command_queue); 
  ret = clFinish(command_queue); 
  ret = clReleaseKernel(kernel); 
  ret = clReleaseProgram(program); 
  //@@ clean up memory on device 
  ret = clReleaseMemObject(deviceA); 
  ret = clReleaseMemObject(deviceB); 
  ret = clReleaseMemObject(deviceC); 

  ret = clReleaseCommandQueue(command_queue); 
  ret = clReleaseContext(context); 
  free(hostA); 
  free(hostB); 
  free(hostC); 
  return EXIT_SUCCESS;
}
