# Matrix Multiplication
## Objective
The purpose of this lab is to get you familiar with mapping computations onto the GPU and multi-dimensional local and global work sizes. You will implement matrix multiplication by writing the GPU kernel code as well as the associated host code.

## Instructions
Given matrices $A \in \mathbb{R}^{n \times m}$ and $B \in \mathbb{R}^{m \times p}$, compute their product $C = AB$. Recall that $C$ will be an $n \times p$ matrix that can be computed with the following formula:

$$
C_{ij} = \sum_{k = 1}^n A_{ik} B_{kj}
$$

Where $M_{ij}$ is the entry at the row $i$ and column $j$ of matrix $M$.

Edit the code to perform the following:

* Set the dimensions of the result matrix
* Create device buffers for the matrices
* Copy host memory to device buffers
* Create the kernel
* Set up arguments for the kernel
* Define local and global work sizes
* Invoke OpenCL kernel
* Copy result matrix from device to host
* Release memory on device
* Write the OpenCL kernel to perform matrix multiplication
* Instructions about where to place each part of the code is demarcated by the `//@@ comment lines`.

## How to Compile
The template.c file contains the host code for the programming assignment. THe kernel.cl file contains the device code. There is a Makefile included which compiles it and links it with library helper functions. It can be run by typing make from the PA3 folder. It generates a solution output file. During development, make sure to run the make clean command before running make.

## How to Test
Use the make run command to test your program on the test cases in the Dataset foldeer. There are a total of 9 tests on which your program will be evaluated.

## Submission
TODO