#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to allocate memory for a matrix
float** allocateMatrix(int rows, int cols) {
    float** matrix = (float**)malloc(rows * sizeof(float*));
    for (int i = 0; i < rows; i++) {
        matrix[i] = (float*)malloc(cols * sizeof(float));
    }
    return matrix;
}

// Function to generate a random float matrix
void generateRandomMatrix(float** matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = (float)rand() / RAND_MAX;
        }
    }
}

// Function to multiply two matrices
float** multiplyMatrices(float** A, int A_rows, int A_cols, float** B, int B_cols) {
    float** C = allocateMatrix(A_rows, B_cols);
    for (int i = 0; i < A_rows; i++) {
        for (int j = 0; j < B_cols; j++) {
            C[i][j] = 0;
            for (int k = 0; k < A_cols; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

// Function to save a matrix to a file
void saveMatrixToFile(const char* filename, float** matrix, int rows, int cols) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file %s!\n", filename);
        exit(1);
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%f ", matrix[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

// Main function
int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s <A> <B> <C>\n", argv[0]);
        return 1;
    }

    int A = atoi(argv[1]);
    int B = atoi(argv[2]);
    int C = atoi(argv[3]);

    // Seed the random number generator
    srand(time(NULL));

    // Allocate and generate random matrices
    float** matrix1 = allocateMatrix(A, B);
    float** matrix2 = allocateMatrix(B, C);
    generateRandomMatrix(matrix1, A, B);
    generateRandomMatrix(matrix2, B, C);

    // Multiply the matrices
    float** matrixProduct = multiplyMatrices(matrix1, A, B, matrix2, C);

    // Save matrices to files
    saveMatrixToFile("A.txt", matrix1, A, B);
    saveMatrixToFile("B.txt", matrix2, B, C);
    saveMatrixToFile("C.txt", matrixProduct, A, C);

    // Free allocated memory
    for (int i = 0; i < A; i++) free(matrix1[i]);
    for (int i = 0; i < B; i++) free(matrix2[i]);
    for (int i = 0; i < A; i++) free(matrixProduct[i]);
    free(matrix1);
    free(matrix2);
    free(matrixProduct);

    return 0;
}
