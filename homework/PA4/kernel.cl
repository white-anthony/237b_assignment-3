__kernel void tiledMatrixMul(__global float* A, __global float* B, __global float* C, int M, int K, int N) {
    const int TS = 16; // Tile size
    // Thread identifiers
    const int row = get_local_id(0); // Local row ID (0..TS-1)
    const int col = get_local_id(1); // Local col ID (0..TS-1)
    const int globalRow = TS * get_group_id(0) + row; // Global row ID in C (0..M-1)
    const int globalCol = TS * get_group_id(1) + col; // Global col ID in C (0..N-1)
 
    // Local memory to fit a tile of TS*TS elements of A and B
    __local float Asub[TS][TS];
    __local float Bsub[TS][TS];
 
    // Initialise the accumulation register
    float acc = 0.0f;
    
    // Loop over all tiles
    const int numTiles = (K + TS - 1) / TS; // Ensure all K elements are covered even if K is not a multiple of TS
    for (int t = 0; t < numTiles; t++) {
        // Load one tile of A and B into local memory
        const int tiledRow = TS * t + row;
        const int tiledCol = TS * t + col;

        if (globalRow < M && tiledCol < K)
            Asub[row][col] = A[globalRow * K + tiledCol];
        else
            Asub[row][col] = 0.0f; // Pad with zero if out-of-bounds

        if (tiledRow < K && globalCol < N)
            Bsub[row][col] = B[tiledRow * N + globalCol];
        else
            Bsub[row][col] = 0.0f; // Pad with zero if out-of-bounds

        // Synchronise to make sure the tile is loaded
        barrier(CLK_LOCAL_MEM_FENCE);
 
        // Perform the computation for a single tile
        for (int k = 0; k < TS; k++) {
            acc += Asub[row][k] * Bsub[k][col];
        }
 
        // Synchronise before loading the next tile
        barrier(CLK_LOCAL_MEM_FENCE);
    }

    // Ensure we don't write out-of-bounds
    if (globalRow < M && globalCol < N) {
        // Store the final result in C
        C[globalRow * N + globalCol] = acc;
    }
}
