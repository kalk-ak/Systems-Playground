# Systems Programming Projects

This repository contains a collection of systems programming projects implemented in C and C++. Each project is self-contained in its own directory and includes a `Makefile` for easy compilation.

Click on a project to explore its source code and detailed documentation.

## Projects

### 📂 [Cache_Simulator](./Cache_Simulator/)

A configurable, event-driven cache simulator written in C/C++. It analyzes the performance of different cache configurations by processing memory access traces. The simulator reports a detailed summary of its performance, including hit/miss rates and total cycles.

### 📂 [Chess](./Chess/)

A complete, command-line chess game written in modern C++. It is built with a clean, object-oriented design, focusing heavily on inheritance and dynamic dispatch to handle complex game logic. It correctly implements all standard chess rules, including check, checkmate, and stalemate detection.

### 📂 [Image_Manipulation](./Image_Manipulation/)

A command-line image processing tool written entirely in C. It reads a source image in the PPM (P6) format, applies a specified transformation (like invert, blur, binarize), and saves the result as a new PPM file.

### 📂 [Image_Rotation](./Image_Rotation/)

An image processing project that includes functions for image rotation and other effects, implemented in both C and assembly language for performance comparison.

### 📂 [Parallel_Quicksort_By_Forking](./Parallel_Quicksort_By_Forking/)

A C-based implementation of a parallel quicksort algorithm. It is designed to sort large files of 64-bit signed integers directly in memory by leveraging `mmap()` to map the file into memory and `fork()` to parallelize the sorting process.
