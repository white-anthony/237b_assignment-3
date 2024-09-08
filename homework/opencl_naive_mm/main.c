#include <stdio.h>
#include <stdlib.h>

#include "device.h"
#include "kernel.h"
#include "matrix.h"

#define CHECK_ERR(err, msg)                           \
    if (err != CL_SUCCESS)                            \
    {                                                 \
        fprintf(stderr, "%s failed: %d\n", msg, err); \
        exit(EXIT_FAILURE);                           \
    }

#define KERNEL_PATH "kernel.cl"

void OpenCLMatrixMultiply(Matrix *input0, Matrix *input1, Matrix *result)
{
    // Load external OpenCL kernel code
    char *kernel_source = OclLoadKernel(KERNEL_PATH); // Load kernel source

    // Device input and output buffers
    cl_mem device_a, device_b, device_c;

    cl_int err;

    cl_device_id device_id;    // device ID
    cl_context context;        // context
    cl_command_queue queue;    // command queue
    cl_program program;        // program
    cl_kernel kernel;          // kernel

    // Find platforms and devices
    OclPlatformProp *platforms = NULL;
    cl_uint num_platforms;

    err = OclFindPlatforms((const OclPlatformProp **)&platforms, &num_platforms);
    CHECK_ERR(err, "OclFindPlatforms");

    // Get ID for first device on first platform
    device_id = platforms[0].devices[0].device_id;

    // Create a context
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    CHECK_ERR(err, "clCreateContext");

    // Create a command queue
    queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
    CHECK_ERR(err, "clCreateCommandQueueWithProperties");

    // Create the program from the source buffer
    program = clCreateProgramWithSource(context, 1, (const char **)&kernel_source, NULL, &err);
    CHECK_ERR(err, "clCreateProgramWithSource");

    // Build the program executable
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    CHECK_ERR(err, "clBuildProgram");

    // Create the compute kernel in the program we wish to run
    kernel = clCreateKernel(program, "matrixMultiply", &err);
    CHECK_ERR(err, "clCreateKernel");

    //@@ Allocate GPU memory here
    device_a = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            sizeof(float) * input0->shape[0] * input0->shape[1],
                            input0->data, &err);
  CHECK_ERR(err, "clCreateBuffer (device_a)");

  device_b = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                            sizeof(float) * input1->shape[0] * input1->shape[1],
                            input1->data, &err);
  CHECK_ERR(err, "clCreateBuffer (device_b)");

  device_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                            sizeof(float) * result->shape[0] * result->shape[1],
                            NULL, &err);
  CHECK_ERR(err, "clCreateBuffer (device_c)");

    //@@ Copy memory to the GPU here
    err = clEnqueueWriteBuffer(queue, device_a, CL_TRUE, 0,
                             sizeof(float) * input0->shape[0] * input0->shape[1],
                             input0->data, 0, NULL, NULL);
  CHECK_ERR(err, "clEnqueueWriteBuffer (device_a)");

  err = clEnqueueWriteBuffer(queue, device_b, CL_TRUE, 0,
                             sizeof(float) * input1->shape[0] * input1->shape[1],
                             input1->data, 0, NULL, NULL);
  CHECK_ERR(err, "clEnqueueWriteBuffer (device_b)");
    // Set the arguments to our compute kernel
    // __global const float *A, __global const float *B, __global float *C,
    // const unsigned int numARows, const unsigned int numAColumns,
    // const unsigned int numBRows, const unsigned int numBColumns,
    // const unsigned int numCRows, const unsigned int numCColumns
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &device_a);
    CHECK_ERR(err, "clSetKernelArg 0");
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &device_b);
    CHECK_ERR(err, "clSetKernelArg 1");
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &device_c);
    CHECK_ERR(err, "clSetKernelArg 2");
    err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &input0->shape[0]);
    CHECK_ERR(err, "clSetKernelArg 3");
    err |= clSetKernelArg(kernel, 4, sizeof(unsigned int), &input0->shape[1]);
    CHECK_ERR(err, "clSetKernelArg 4");
    err |= clSetKernelArg(kernel, 5, sizeof(unsigned int), &input1->shape[0]);
    CHECK_ERR(err, "clSetKernelArg 5");
    err |= clSetKernelArg(kernel, 6, sizeof(unsigned int), &input1->shape[1]);
    CHECK_ERR(err, "clSetKernelArg 6");
    err |= clSetKernelArg(kernel, 7, sizeof(unsigned int), &result->shape[0]);
    CHECK_ERR(err, "clSetKernelArg 7");
    err |= clSetKernelArg(kernel, 8, sizeof(unsigned int), &result->shape[1]);
    CHECK_ERR(err, "clSetKernelArg 8");

    // @@ define local and global work sizes
    size_t max_work_group_size;
   clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &max_work_group_size, NULL);

  size_t local_work_size[2] = {16, 16};
  size_t global_work_size[2] = {
    ((result->shape[0] - 1) / local_work_size[0] + 1) * local_work_size[0],
    ((result->shape[1] - 1) / local_work_size[1] + 1) * local_work_size[1]
};


    //@@ Launch the GPU Kernel here
  err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, &global_work_size,
                              &local_work_size, 0, NULL, NULL);
  CHECK_ERR(err, "clEnqueueNDRangeKernel");
    //@@ Copy the GPU memory back to the CPU here
  err = clEnqueueReadBuffer(queue, device_c, CL_TRUE, 0,
                             sizeof(float) * result->shape[0] * result->shape[1],
                             result->data, 0, NULL, NULL);
  CHECK_ERR(err, "clEnqueueReadBuffer");
    //@@ Free the GPU memory here
  clReleaseMemObject(device_a);
  clReleaseMemObject(device_b);
  clReleaseMemObject(device_c);
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <input_file_0> <input_file_1> <answer_file> <output_file>\n", argv[0]);
        return -1;
    }

    const char *input_file_a = argv[1];
    const char *input_file_b = argv[2];
    const char *input_file_c = argv[3];
    const char *input_file_d = argv[4];

    // Host input and output vectors and sizes
    Matrix host_a, host_b, host_c, answer;
    
    cl_int err;

    err = LoadMatrix(input_file_a, &host_a);
    CHECK_ERR(err, "LoadMatrix");

    err = LoadMatrix(input_file_b, &host_b);
    CHECK_ERR(err, "LoadMatrix");

    err = LoadMatrix(input_file_c, &answer);
    CHECK_ERR(err, "LoadMatrix");

    int rows, cols;
    //@@ Update these values for the output rows and cols of the output
    //@@ Do not use the results from the answer matrix
    rows = host_a.shape[0];
    cols = host_b.shape[1];

    // Allocate the memory for the target.
    host_c.shape[0] = rows;
    host_c.shape[1] = cols;
    host_c.data = (float *)malloc(sizeof(float) * host_c.shape[0] * host_c.shape[1]);

    // Call your matrix multiply.
    OpenCLMatrixMultiply(&host_a, &host_b, &host_c);

    // // Call to print the matrix
    // PrintMatrix(&host_c);

    // Save the matrix
    SaveMatrix(input_file_d, &host_c);

    // Check the result of the matrix multiply
    CheckMatrix(&answer, &host_c);

    // Release host memory
    free(host_a.data);
    free(host_b.data);
    free(host_c.data);
    free(answer.data);

    return 0;
}