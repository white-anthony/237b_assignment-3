__kernel void matrixMultiply(
    __global const float *A, __global const float *B, __global float *C,
    const unsigned int numARows, const unsigned int numAColumns,
    const unsigned int numBRows, const unsigned int numBColumns,
    const unsigned int numCRows, const unsigned int numCColumns) {

    const int tileSize = 16;

    int globalRow = get_global_id(0);
    int globalCol = get_global_id(1);

    int localRow = get_local_id(0);
    int localCol = get_local_id(1);

    __local float Atile[tileSize][tileSize];
    __local float Btile[tileSize][tileSize];

    float acc = 0.0f;

    for (int k = 0; k < (numAColumns + tileSize - 1) / tileSize; k++) {

        if (globalRow < numARows && (k * tileSize + localCol) < numAColumns)
            Atile[localRow][localCol] = A[globalRow * numAColumns + k * tileSize + localCol];
        else
            Atile[localRow][localCol] = 0.0f;

        if (globalCol < numBColumns && (k * tileSize + localRow) < numBRows)
            Btile[localRow][localCol] = B[(k * tileSize + localRow) * numBColumns + globalCol];
        else
            Btile[localRow][localCol] = 0.0f;

        barrier(CLK_LOCAL_MEM_FENCE);

        for (int n = 0; n < tileSize; n++) {
            acc += Atile[localRow][n] * Btile[n][localCol];
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    if (globalRow < numCRows && globalCol < numCColumns) {
        C[globalRow * numCColumns + globalCol] = acc;
    }
}
