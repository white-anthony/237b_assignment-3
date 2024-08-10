#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <math.h>

#define TILE_SIZE 16

// Function to allocate memory for a matrix
float* allocateMatrix(int rows, int cols) {
    return (float*)malloc(rows * cols * sizeof(float));
}

// Function to read a matrix from a file
void readMatrixFromFile(const char* filename, float* matrix, int rows, int cols) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file %s!\n", filename);
        exit(1);
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fscanf(file, "%f", &matrix[i * cols + j]);
        }
    }
    fclose(file);
}

// Function to verify the result
void verifyResult(float* result, float* expected, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (fabs(result[i * cols + j] - expected[i * cols + j]) > 1e-3) {
                printf("Verification failed at element (%d, %d): %f != %f\n", i, j, result[i * cols + j], expected[i * cols + j]);
                return;
            }
        }
    }
    printf("Verification succeeded!\n");
}

// Function to read kernel source from a file
char* readKernelSource(const char* kernelFile) {
    FILE* file = fopen(kernelFile, "r");
    if (!file) {
        printf("Error opening kernel source file %s!\n", kernelFile);
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    char* source = (char*)malloc(size + 1);
    fread(source, 1, size, file);
    source[size] = '\0';
    fclose(file);
    return source;
}

void printMatrix(const char* name, float* matrix, int rows, int cols) {
    printf("Matrix %s:\n", name);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%f ", matrix[i * cols + j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char* argv[]) {
    if (argc != 8) {
        printf("Usage: %s <A_rows> <A_cols_B_rows> <B_cols> <A_file> <B_file> <C_file> <kernel_file>\n", argv[0]);
        return 1;
    }

    int A_rows = atoi(argv[1]);
    int A_cols_B_rows = atoi(argv[2]);
    int B_cols = atoi(argv[3]);
    const char* A_file = argv[4];
    const char* B_file = argv[5];
    const char* C_file = argv[6];
    const char* kernel_file = argv[7];

    // Allocate matrices
    float* A = allocateMatrix(A_rows, A_cols_B_rows);
    float* B = allocateMatrix(A_cols_B_rows, B_cols);
    float* C = allocateMatrix(A_rows, B_cols);
    float* C_expected = allocateMatrix(A_rows, B_cols);

    // Read matrices from files
    readMatrixFromFile(A_file, A, A_rows, A_cols_B_rows);
    readMatrixFromFile(B_file, B, A_cols_B_rows, B_cols);
    readMatrixFromFile(C_file, C_expected, A_rows, B_cols);

    // Only use these on smaller matrices, may cause terminal issues
    // printMatrix("A", A, A_rows, A_cols_B_rows);
    // printMatrix("B", B, A_cols_B_rows, B_cols);
    // printMatrix("C_expected", C_expected, A_rows, B_cols);

    // Read kernel source from file
    char* kernelSource = readKernelSource(kernel_file);

    // OpenCL setup
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_context context = NULL;
    cl_command_queue queue = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;
    cl_int ret;

    ret = clGetPlatformIDs(1, &platform_id, NULL);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    queue = clCreateCommandQueue(context, device_id, 0, &ret);

    cl_mem d_A = clCreateBuffer(context, CL_MEM_READ_ONLY, A_rows * A_cols_B_rows * sizeof(float), NULL, &ret);
    cl_mem d_B = clCreateBuffer(context, CL_MEM_READ_ONLY, A_cols_B_rows * B_cols * sizeof(float), NULL, &ret);
    cl_mem d_C = clCreateBuffer(context, CL_MEM_WRITE_ONLY, A_rows * B_cols * sizeof(float), NULL, &ret);

    ret = clEnqueueWriteBuffer(queue, d_A, CL_TRUE, 0, A_rows * A_cols_B_rows * sizeof(float), A, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(queue, d_B, CL_TRUE, 0, A_cols_B_rows * B_cols * sizeof(float), B, 0, NULL, NULL);

    program = clCreateProgramWithSource(context, 1, (const char**)&kernelSource, NULL, &ret);
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    kernel = clCreateKernel(program, "tiledMatrixMul", &ret);

    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&d_A);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&d_B);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void*)&d_C);
    ret = clSetKernelArg(kernel, 3, sizeof(int), (void*)&A_rows);
    ret = clSetKernelArg(kernel, 4, sizeof(int), (void*)&A_cols_B_rows);
    ret = clSetKernelArg(kernel, 5, sizeof(int), (void*)&B_cols);

    size_t global[2] = { (size_t)((B_cols + TILE_SIZE - 1) / TILE_SIZE) * TILE_SIZE, (size_t)((A_rows + TILE_SIZE - 1) / TILE_SIZE) * TILE_SIZE };
    size_t local[2] = { TILE_SIZE, TILE_SIZE };

    ret = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, local, 0, NULL, NULL);
    ret = clFinish(queue);

    ret = clEnqueueReadBuffer(queue, d_C, CL_TRUE, 0, A_rows * B_cols * sizeof(float), C, 0, NULL, NULL);

    verifyResult(C, C_expected, A_rows, B_cols);

    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(d_A);
    ret = clReleaseMemObject(d_B);
    ret = clReleaseMemObject(d_C);
    ret = clReleaseCommandQueue(queue);
    ret = clReleaseContext(context);

    free(A);
    free(B);
    free(C);
    free(C_expected);
    free(kernelSource);

    return 0;
}
