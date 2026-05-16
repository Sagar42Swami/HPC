# C++ OpenMP Practical Execution Guide

This folder contains separate C++ OpenMP solutions for the practical statements.

## Files

- `01_parallel_bfs_dfs.cpp`: Parallel BFS and DFS using OpenMP.
- `02_parallel_bubble_merge_sort.cpp`: Sequential and parallel Bubble Sort and Merge Sort.
- `03_parallel_reduction.cpp`: Min, Max, Sum, and Average using OpenMP reduction.
- `04_vector_add_matrix_multiply_openmp.cpp`: Vector addition and matrix multiplication using OpenMP.

## Step 1: Install a Compiler

You need `g++` with OpenMP support.

### Windows

1. Install MSYS2 from `https://www.msys2.org/`.
2. Open the MSYS2 MinGW terminal.
3. Run this command:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
```

4. Add the MinGW `bin` folder to your Windows PATH if needed.
5. Check installation:

```bash
g++ --version
```

### Linux

Run:

```bash
sudo apt update
sudo apt install g++
```

### macOS

Install LLVM with OpenMP:

```bash
brew install llvm libomp
```

## Step 2: Open Terminal in This Folder

Go to the practical folder:

```bash
cd E:/Sem_8/high-performance-computing/openmp_practicals
```

If PowerShell does not accept forward slashes, use:

```powershell
cd E:\Sem_8\high-performance-computing\openmp_practicals
```

## Step 3: Compile a Program

The important OpenMP flag is:

```bash
-fopenmp
```

It tells the compiler to enable OpenMP.

### Compile Practical 1

```bash
g++ -fopenmp 01_parallel_bfs_dfs.cpp -o 01_parallel_bfs_dfs
```

### Compile Practical 2

```bash
g++ -fopenmp 02_parallel_bubble_merge_sort.cpp -o 02_parallel_bubble_merge_sort
```

### Compile Practical 3

```bash
g++ -fopenmp 03_parallel_reduction.cpp -o 03_parallel_reduction
```

### Compile Practical 4

```bash
g++ -fopenmp 04_vector_add_matrix_multiply_openmp.cpp -o 04_vector_add_matrix_multiply_openmp
```

## Step 4: Run a Program

### Run Practical 1

```bash
./01_parallel_bfs_dfs
```

Sample input:

```text
6
7
0 1
0 2
1 3
1 4
2 4
3 5
4 5
0
```

### Run Practical 2

```bash
./02_parallel_bubble_merge_sort
```

Sample input:

```text
10000
```

Use small input like `10` if you want to see the array printed.

### Run Practical 3

```bash
./03_parallel_reduction
```

Sample input:

```text
1000000
```

Use small input like `10` if you want to see the array printed.

### Run Practical 4

```bash
./04_vector_add_matrix_multiply_openmp
```

Sample input:

```text
1000000
300
```

Use small values like vector size `5` and matrix size `3` if you want printed output.

## Step 5: Set Number of OpenMP Threads

OpenMP can use multiple CPU threads.

### PowerShell

```powershell
$env:OMP_NUM_THREADS=4
```

Then run the program:

```powershell
.\03_parallel_reduction.exe
```

### Linux or MSYS2 Bash

```bash
export OMP_NUM_THREADS=4
```

Then run the program:

```bash
./03_parallel_reduction
```

## Common Exam Explanation

OpenMP is used for shared-memory parallel programming.

The header file is:

```cpp
#include <omp.h>
```

The compile flag is:

```bash
-fopenmp
```

The common directive for loop parallelization is:

```cpp
#pragma omp parallel for
```

Reduction combines thread-local results safely:

```cpp
#pragma omp parallel for reduction(+:sum)
```

Timing is measured using:

```cpp
omp_get_wtime()
```

## Troubleshooting

If `g++` is not recognized, the compiler is not installed or not added to PATH.

If OpenMP errors appear, confirm that you compiled using `-fopenmp`.

If Windows creates `.exe` files, run them like:

```powershell
.\01_parallel_bfs_dfs.exe
```

If Linux or MSYS2 creates files without `.exe`, run them like:

```bash
./01_parallel_bfs_dfs
```
