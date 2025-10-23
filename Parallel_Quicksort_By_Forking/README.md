# C++ Parallel Quicksort with mmap() and fork()

## Project Summary

This project is a C-based implementation of a parallel quicksort algorithm. It is designed to sort large files of 64-bit signed integers directly in memory by leveraging two core Unix systems programming concepts:

1. **`mmap()`**: The target file is mapped directly into the process's virtual address space. This treats the entire file like a large array in memory, avoiding costly `read()` and `write()` system calls.
2. **`fork()`**: The recursive sorting steps are parallelized by creating child processes to handle independent partitions of the data. This follows a classic "fork/join" model to utilize multiple CPU cores.

The program reads a binary file, sorts its contents in-place, and provides a performance report based on different levels of parallelism.

---

## Core Concepts & Technologies

- **Programming Language:** **C** (C99)
- **Build System:** `make` / `Makefile`
- **Parallelism:** Process-based parallelism using `fork()` and `waitpid()`.
- **Memory Management:** `mmap()` for shared memory file mapping and `munmap()` to release the mapping.
- **File I/O:** `open()`, `fstat()`, and `close()` to set up the memory map.

---

## How to Use

### 1. Compile

A `Makefile` is provided. Simply run `make` to build the `parsort` executable, along with the helper utilities `gen_rand_data` (for creating test files) and `seqsort` (a sequential sorter for verification).

```bash
make
```

### 2. Generate Test Data (Optional)

Use the `gen_rand_data` utility to create a binary file of a specified size (e.g., `16M` for 16 megabytes).

```bash
# Syntax: ./gen_rand_data <size> <filename>
./gen_rand_data 16M data_file.bin
```

### 3. Run the Sorter

Run the `parsort` executable, providing the data file and a **parallel threshold**.

- `<data-file>`: The path to the binary file to be sorted.
- `<parallel-threshold>`: An integer. When a sorting task has _more_ elements than this threshold, it will be split and run in parallel using child processes. Tasks with fewer elements will be sorted sequentially.

```bash
# Syntax: ./parsort <data-file> <parallel-threshold>
./parsort data_file.bin 65536
```

---

## Example Usage & Verification

The program sorts the data file **in-place**. There is no standard output on success.

To verify correctness, you can compare its output to the provided `seqsort` "oracle" program.

```bash
# 1. Generate two identical data files
./gen_rand_data 16M file_A.bin
cp file_A.bin file_B.bin

# 2. Sort one file with your parallel sorter
./parsort file_A.bin 65536

# 3. Sort the other with the (trusted) sequential sorter
./seqsort file_B.bin

# 4. Compare them. If no output is produced, they are identical!
diff file_A.bin file_B.bin

# 5. Check the exit code of diff (0 means files are the same)
echo $?
# [ 0 ]
```

---

## Performance Benchmark

This section contains the performance analysis from sorting a 16M test file with various parallel thresholds.

**Test System:** `Unix Sysytem`
**Data File Size:** 16M (16,777,216 bytes)
**Total 64-bit Integers:** 2,097,152

### Timing Results (Real Time)

| Parallel Threshold (Elements) | Real Time (s)     |
| :---------------------------- | :---------------- |
| `2097152` (Sequential)        | `[Userfills in]`  |
| `1048576`                     | `[User fills in]` |
| `524288`                      | `[User fills in]` |
| `262144`                      | `[User fills in]` |
| `131072`                      | `[User fills in]` |
| `65536`                       | `[User fills in]` |
| `32768`                       | `[User fills in]` |
| `16384`                       | `[User fills in]` |

### Analysis

`[Placeholder: Write your analysis here. Explain the trend you see in the`real`time as the threshold decreases. Why does the performance improve? Does it eventually stop improving or get worse? Why would that be? Consider the number of processes created, the overhead of`fork()`, and the number of available CPU cores.]`

---

## Run The Benchmark Yourself! 🔬

You can easily reproduce these tests. The repository includes the `run_experiments.sh` script used to generate the timings.

### 1. The Script (`run_experiments.sh`)

This script creates a 16M test file, then repeatedly copies it and runs `parsort` with the `time` command for each threshold, from fully sequential down to a threshold of 16384.

```bash
#!/bin/bash

# Clean and build the project
make clean
make

# Create a temporary directory for test data
mkdir -p /tmp/$(whoami)
DATA_FILE="/tmp/$(whoami)/data_16M.in"
TEST_FILE="/tmp/$(whoami)/test_16M.in"

echo "Generating 16M test file..."
./gen_rand_data 16M $DATA_FILE

# Define the thresholds to test
THRESHOLDS=(2097152 1048576 524288 262144 131072 65536 32768 16384)

echo "Running benchmark..."
echo "---------------------------------"

for THRESHOLD in "${THRESHOLDS[@]}"
do
    echo "Testing with threshold: $THRESHOLD"
    # Copy fresh data for an in-place sort
    cp $DATA_FILE $TEST_FILE

    # Run the time command
    # We pipe stderr to stdout and use grep to isolate the 'real' time
    { time ./parsort $TEST_FILE $THRESHOLD 2>&1; } | grep 'real'
    echo "---------------------------------"
done

echo "Benchmark complete."
rm -rf /tmp/$(whoami)
```

### 2. How to Run

After cloning the repository, just make the script executable and run it:

```bash
# 1. Make sure you've compiled the project
make

# 2. Give the script execute permissions
chmod +x ./run_experiments.sh

# 3. Run the benchmark!
./run_experiments.sh
```
