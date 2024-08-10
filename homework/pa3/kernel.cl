__kernel void matrix_multiply(
  __global const float *A, __global const float *B, __global float *C,
  const int numARows, const int numAColumns,
  const int numBRows, const int numBColumns,
  const int numCRows, const int numCColumns)
{
  int col = get_global_id(1);
  int row = get_global_id(0);

  if (row < numARows && col < numBColumns) {
    float sum = 0;
    for (int k = 0; k < numAColumns; ++k) { 
      sum += A[row * numAColumns + k] * B[k * numBColumns + col];
    }
    C[row * numCColumns + col] = sum;
  }
}
 
