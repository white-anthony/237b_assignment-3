#!/bin/bash

# Compile the C program
gcc -o datagen datagen.c

# Create the overall directory for the output files
output_dir="matrices"
mkdir -p "$output_dir"

# Define the array of arguments
declare -a args=(
    "16 16 16"
    "64 64 64"
    "64 128 64"matrix
    "84 84 84"
    "2048 2048 2048"
)

# Run the program 10 times with different arguments
for i in {0..4}; do
    run_dir="$output_dir/run_$((i+1))"
    mkdir -p "$run_dir"
    
    # Run the compiled program with the current set of arguments
    ./datagen ${args[i]}

    # Move the output files to the respective directory
    mv A.txt "$run_dir/A.txt"
    mv B.txt "$run_dir/B.txt"
    mv C.txt "$run_dir/C.txt"
done

echo "All runs completed. Output files are saved in the '$output_dir' directory."
