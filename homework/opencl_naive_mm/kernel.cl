__kernel void matrixMultiply(
    __global const float *A, __global const float *B, __global float *C,
    const unsigned int numARows, const unsigned int numAColumns,
    const unsigned int numBRows, const unsigned int numBColumns,
    const unsigned int numCRows, const unsigned int numCColumns) {

    int globalRow = get_global_id(0);
    int globalCol = get_global_id(1);

    if (globalRow >= numCRows || globalCol >= numCColumns) {
        return;
    }

    float sum = 0.0f;

    for (int k = 0; k < numAColumns; ++k) {
        sum += A[globalRow * numAColumns + k] * B[k * numBColumns + globalCol];
    }

    C[globalRow * numCColumns + globalCol] = sum;
}

