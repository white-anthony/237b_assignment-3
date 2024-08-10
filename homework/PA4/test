#!/bin/bash

# Path to the compiled matrix multiplication program
matrix_multiply_exec="./tmm"

# Define the array of arguments used for generating the matrices
declare -a args=(
    "16 16 16"
    "64 64 64"
    "64 128 64"matrix
    "84 84 84"
    "2048 2048 2048"
)

# Path to the kernel file
kernel_file="kernel.cl"

# Directory containing the generated matrices
output_dir="matrices"

# Timing log file
timing_log="timing_log.txt"
echo "Matrix Multiplication Timing Log" > $timing_log
echo "================================" >> $timing_log

# Loop over each set of matrices and perform matrix multiplication
for i in {0..4}; do
    run_dir="$output_dir/run_$((i+1))"
    
    # Parse the arguments
    read -r A_rows A_cols_B_rows B_cols <<< "${args[i]}"
    
    # File paths for matrices A, B, and expected C
    A_file="$run_dir/A.txt"
    B_file="$run_dir/B.txt"
    C_file="$run_dir/C.txt"
    
    # Start time
    start_time=$(date +%s.%N)
    
    # Run the matrix multiplication program
    $matrix_multiply_exec $A_rows $A_cols_B_rows $B_cols $A_file $B_file $C_file $kernel_file
    
    # End time
    end_time=$(date +%s.%N)
    
    # Calculate duration
    duration=$(echo "$end_time - $start_time" | bc)
    
    # Log the duration
    echo "Run $((i+1)): ${args[i]} -> ${duration} seconds" >> $timing_log
done

echo "All matrix multiplications completed. Timing details are saved in '$timing_log'."
