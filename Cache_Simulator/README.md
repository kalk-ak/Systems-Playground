# C/C++ Command-Line Cache Simulator

## Project Summary

This project is a configurable, event-driven cache simulator written in C/C++. It is designed to analyze the performance of different cache configurations by processing memory access traces.

The simulator is built from scratch in C/C++ and compiles with `make`. It reads a trace file from standard input, where each line represents a memory operation (`l` for load, `s` for store) and a memory address. Based on user-defined parameters for the cache's geometry and policies, it simulates the cache's behavior and reports a detailed summary of its performance, including hit/miss rates and total cycles.

---

## Technologies Used

* **Programming Language:** C / C++ (C++ Standard Library or C Standard Library only)
* **Build System:** `make` / `Makefile`
* **Compiler:** `gcc` / `g++` with `-Wall -Wextra -pedantic` flags
* **Development Tools:** `Valgrind` (for memory-safe development), `gdb`

---

## Simulated Features

This simulator can be configured to model a wide variety of cache architectures:

* **Cache Geometries:** Simulates any cache with power-of-2 sizes for:
    * Number of sets
    * Number of blocks per set (associativity)
    * Block size (in bytes)
* **Write Policies:**
    * `write-through` (writes go to both cache and memory)
    * `write-back` (writes go to cache; memory is updated on eviction)
* **Write Allocation Policies:**
    * `write-allocate` (on a write miss, the block is loaded into the cache first)
    * `no-write-allocate` (on a write miss, only memory is written)
* **Eviction Policies:**
    * `lru` (Least Recently Used)
    * `fifo` (First-In, First-Out)
* **Performance Metrics:**
    * Tracks all loads, stores, hits, and misses.
    * Calculates total cycles based on cache/memory access penalties.

---

## How to Use

### 1. Compile

Navigate to the project directory and use `make` to compile the program. This will create the executable `csim`.

```bash
make
```

### 2. Run

The program is run from the command line with 6 arguments and reads a trace file from standard input using a redirect (`<`).

**Syntax:**

```bash
./csim <num-sets> <num-blocks> <block-size> <write-allocate> <write-through> <eviction-policy> < <path/to/trace.txt>
```

**Argument Definitions:**

* `<num-sets>`: The number of sets (e.g., `256`)
* `<num-blocks>`: The number of blocks per set (associativity, e.g., `4`)
* `<block-size>`: The size of each block in bytes (e.g., `16`)
* `<write-allocate>`: `write-allocate` or `no-write-allocate`
* `<write-through>`: `write-through` or `write-back`
* `<eviction-policy>`: `lru` or `fifo`

---

## Example Usage & Output

Here is an example of running the simulator with a 4-way set-associative cache and a `write-back` policy, using the `gcc.trace` file.

### Example Command

```bash
./csim 256 4 16 write-allocate write-back lru < traces/gcc.trace
```

### Example Output (Placeholder)

The program will process the entire trace file and print the following 7-line summary to the console. This is the "placeholder" for what your program will produce.

```
Total loads: [COUNT]
Total stores: [COUNT]
Load hits: [COUNT]
Load misses: [COUNT]
Store hits: [COUNT]
Store misses: [COUNT]
Total cycles: [COUNT]
```
