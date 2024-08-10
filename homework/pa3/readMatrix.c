#include <stdio.h>
#include <stdlib.h> 
#include "readMatrix.h"

/**
 * @brief Read a file representing a matrix into a float array.
 *
 * @param filename The file representing the matrix. 
 * The file must consist of whitespace-separated numbers. 
 * The first two numbers must be integers representing the number of rows and columns respectively. 
 * The subsequent (rows * cols) entries must be floats stored in row-major order.
 * @param M A pointer to a float array that will hold the elements of the matrix. 
 * The caller is responsible for freeing *M.
 * @param rows The address to store the number of rows. 
 * @param cols The address to store the number of columns.
 *
 * @return SUCCESS if the relevant variables are initialized. FAILURE otherwise.
 */
errcode readMatrix(const char* filename, float** M, int* rows, int* cols) {
  FILE* fp = fopen(filename, "r");
  if (NULL == fp) {
    fprintf(stderr, "ERROR: File %s could not be opened.\n", filename);
    return FAILURE; 
  }

  // read the number of rows and columns
  if (fscanf(fp, "%i %i", rows, cols) != 2) {
    fprintf(stderr, "ERROR: Could not read dimensions of matrix in file %s.\n", filename);
    return FAILURE; 
  }
  
  // total number of entries in the matrix
  size_t numentries = *rows * *cols;
  // allocate memory for matrix
  *M = (float*)malloc(numentries * sizeof(float));
  if (NULL == *M) {
    perror("Could not allocate memory for the matrix.\n");
    return FAILURE; 
  }

  // fill matrix array
  for (int i = 0; i < numentries; ++i) {
    if (fscanf(fp, "%f", &((*M)[i])) != 1) {
      fprintf(stderr, "ERROR: Could not read all entries of matrix in file %s.\n", filename);
      return FAILURE;
    }
    // printf("i = %d, read value %f\n", i, (*M)[i]);
  }

  fclose(fp);
  return SUCCESS;
}

// example of usage 
/*
int main() {
  char* filename = "Dataset/1/input0.raw";
  // initialize variables 
  // initialized to invalid values for safety reasons
  float* M = NULL;
  int rows = -1; 
  int cols = -1; 
  if (readMatrix(filename, &M, &rows, &cols) != SUCCESS) {
    fprintf(stderr, "Failed to read matrix.\n"); 
    exit(EXIT_FAILURE);
  }
  
  // print the matrix
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      fprintf(stdout, "%f ", M[i * cols + j]); 
    }
    fprintf(stdout, "\n"); 
  }
  
  // don't forget to free the matrix
  free(M);
  return 0;
}*/
