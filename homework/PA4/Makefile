# Makefile

# Compiler and flags
CC = gcc
CFLAGS = -g

# Executables
TMM_EXEC = tmm
DATAGEN_EXEC = datagen

# Source files
TMM_SRC = tmm.c
DATAGEN_SRC = datagen.c

# Scripts
TMM_SCRIPT = test.sh
DATAGEN_SCRIPT = datagen.sh

# Directories and files
OUTPUT_DIR = matrices
TIMING_LOG = timing_log.txt

# Default rule to compile and run the matrix multiplication
.PHONY: run
run: $(TMM_EXEC)
	@echo "Running matrix multiplication test script..."
	@chmod +x $(TMM_SCRIPT)
	@./$(TMM_SCRIPT)

# Rule to compile the matrix multiplication program
$(TMM_EXEC): $(TMM_SRC)
	@echo "Compiling $(TMM_SRC) to create $(TMM_EXEC)..."
	$(CC) -o $(TMM_EXEC) $(TMM_SRC) -lOpenCL $(CFLAGS)


# Rule to compile the data generation program
$(DATAGEN_EXEC): $(DATAGEN_SRC)
	@echo "Compiling $(DATAGEN_SRC) to create $(DATAGEN_EXEC)..."
	$(CC) -o $(DATAGEN_EXEC) $(DATAGEN_SRC)

# Rule to clean up all generated files
.PHONY: clean
clean:
	@echo "Cleaning up generated files..."
	@rm -f $(TMM_EXEC)
	@rm -f $(TIMING_LOG)

# Print help message
.PHONY: help
help:
	@echo "Makefile targets:"
	@echo "  run        - Compile and run the matrix multiplication program and test script"
	@echo "  datagen    - Compile and run the data generation program and script"
	@echo "  clean      - Remove all generated files and executables"
