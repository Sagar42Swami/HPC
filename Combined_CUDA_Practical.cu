#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cuda_runtime.h>
#include <stdbool.h>

#define CUDA_CHECK(call)                                                     \
    do {                                                                     \
        cudaError_t error = call;                                             \
        if (error != cudaSuccess) {                                           \
            printf("CUDA error at line %d: %s\n", __LINE__,                  \
                   cudaGetErrorString(error));                               \
            exit(1);                                                         \
        }                                                                    \
    } while (0)

__global__ void vectorAddKernel(int *a, int *b, int *c, int size) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    if (tid < size) {
        c[tid] = a[tid] + b[tid];
    }
}

void vectorAddCPU(int *a, int *b, int *c, int size) {
    for (int i = 0; i < size; i++) {
        c[i] = a[i] + b[i];
    }
}

bool verifyVector(int *gpuResult, int *cpuResult, int size) {
    for (int i = 0; i < size; i++) {
        if (gpuResult[i] != cpuResult[i]) {
            return false;
        }
    }
    return true;
}

__global__ void matrixMulKernel(int *a, int *b, int *c,
                                int rowsA, int colsA, int colsB) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < rowsA && col < colsB) {
        int sum = 0;

        for (int k = 0; k < colsA; k++) {
            sum += a[row * colsA + k] * b[k * colsB + col];
        }

        c[row * colsB + col] = sum;
    }
}

void matrixMulCPU(int *a, int *b, int *c,
                  int rowsA, int colsA, int colsB) {
    for (int row = 0; row < rowsA; row++) {
        for (int col = 0; col < colsB; col++) {
            int sum = 0;

            for (int k = 0; k < colsA; k++) {
                sum += a[row * colsA + k] * b[k * colsB + col];
            }

            c[row * colsB + col] = sum;
        }
    }
}

bool verifyMatrix(int *gpuResult, int *cpuResult, int rows, int cols) {
    for (int i = 0; i < rows * cols; i++) {
        if (gpuResult[i] != cpuResult[i]) {
            return false;
        }
    }
    return true;
}

void runVectorAddition(void) {
    int size = 1000000;
    int *a, *b, *cGpu, *cCpu;
    int *devA, *devB, *devC;

    a = (int *)malloc(size * sizeof(int));
    b = (int *)malloc(size * sizeof(int));
    cGpu = (int *)malloc(size * sizeof(int));
    cCpu = (int *)malloc(size * sizeof(int));

    if (a == NULL || b == NULL || cGpu == NULL || cCpu == NULL) {
        printf("Host memory allocation failed.\n");
        exit(1);
    }

    for (int i = 0; i < size; i++) {
        a[i] = rand() % 1000;
        b[i] = rand() % 1000;
    }

    CUDA_CHECK(cudaMalloc((void **)&devA, size * sizeof(int)));
    CUDA_CHECK(cudaMalloc((void **)&devB, size * sizeof(int)));
    CUDA_CHECK(cudaMalloc((void **)&devC, size * sizeof(int)));

    CUDA_CHECK(cudaMemcpy(devA, a, size * sizeof(int), cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(devB, b, size * sizeof(int), cudaMemcpyHostToDevice));

    clock_t startGpu = clock();

    int blockSize = 256;
    int gridSize = (size + blockSize - 1) / blockSize;
    vectorAddKernel<<<gridSize, blockSize>>>(devA, devB, devC, size);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    clock_t endGpu = clock();
    double gpuTime = (double)(endGpu - startGpu) / CLOCKS_PER_SEC;

    CUDA_CHECK(cudaMemcpy(cGpu, devC, size * sizeof(int), cudaMemcpyDeviceToHost));

    clock_t startCpu = clock();
    vectorAddCPU(a, b, cCpu, size);
    clock_t endCpu = clock();
    double cpuTime = (double)(endCpu - startCpu) / CLOCKS_PER_SEC;

    printf("\n--- Vector Addition ---\n");
    printf("Vector size       : %d\n", size);
    printf("GPU time          : %f seconds\n", gpuTime);
    printf("CPU time          : %f seconds\n", cpuTime);
    printf("Output match      : %s\n", verifyVector(cGpu, cCpu, size) ? "True" : "False");
    printf("Speedup factor    : %f\n", cpuTime / gpuTime);

    CUDA_CHECK(cudaFree(devA));
    CUDA_CHECK(cudaFree(devB));
    CUDA_CHECK(cudaFree(devC));

    free(a);
    free(b);
    free(cGpu);
    free(cCpu);
}

void runMatrixMultiplication(void) {
    int rowsA = 512;
    int colsA = 512;
    int rowsB = 512;
    int colsB = 512;
    int *a, *b, *cGpu, *cCpu;
    int *devA, *devB, *devC;

    if (colsA != rowsB) {
        printf("Matrix multiplication is not possible.\n");
        return;
    }

    a = (int *)malloc(rowsA * colsA * sizeof(int));
    b = (int *)malloc(rowsB * colsB * sizeof(int));
    cGpu = (int *)malloc(rowsA * colsB * sizeof(int));
    cCpu = (int *)malloc(rowsA * colsB * sizeof(int));

    if (a == NULL || b == NULL || cGpu == NULL || cCpu == NULL) {
        printf("Host memory allocation failed.\n");
        exit(1);
    }

    for (int i = 0; i < rowsA * colsA; i++) {
        a[i] = rand() % 100;
    }

    for (int i = 0; i < rowsB * colsB; i++) {
        b[i] = rand() % 100;
    }

    CUDA_CHECK(cudaMalloc((void **)&devA, rowsA * colsA * sizeof(int)));
    CUDA_CHECK(cudaMalloc((void **)&devB, rowsB * colsB * sizeof(int)));
    CUDA_CHECK(cudaMalloc((void **)&devC, rowsA * colsB * sizeof(int)));

    CUDA_CHECK(cudaMemcpy(devA, a, rowsA * colsA * sizeof(int), cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemcpy(devB, b, rowsB * colsB * sizeof(int), cudaMemcpyHostToDevice));

    clock_t startGpu = clock();

    dim3 blockSize(16, 16);
    dim3 gridSize((colsB + blockSize.x - 1) / blockSize.x,
                  (rowsA + blockSize.y - 1) / blockSize.y);
    matrixMulKernel<<<gridSize, blockSize>>>(devA, devB, devC, rowsA, colsA, colsB);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    clock_t endGpu = clock();
    double gpuTime = (double)(endGpu - startGpu) / CLOCKS_PER_SEC;

    CUDA_CHECK(cudaMemcpy(cGpu, devC, rowsA * colsB * sizeof(int), cudaMemcpyDeviceToHost));

    clock_t startCpu = clock();
    matrixMulCPU(a, b, cCpu, rowsA, colsA, colsB);
    clock_t endCpu = clock();
    double cpuTime = (double)(endCpu - startCpu) / CLOCKS_PER_SEC;

    printf("\n--- Matrix Multiplication ---\n");
    printf("Matrix A size     : %d x %d\n", rowsA, colsA);
    printf("Matrix B size     : %d x %d\n", rowsB, colsB);
    printf("Matrix C size     : %d x %d\n", rowsA, colsB);
    printf("GPU time          : %f seconds\n", gpuTime);
    printf("CPU time          : %f seconds\n", cpuTime);
    printf("Output match      : %s\n", verifyMatrix(cGpu, cCpu, rowsA, colsB) ? "True" : "False");
    printf("Speedup factor    : %f\n", cpuTime / gpuTime);

    CUDA_CHECK(cudaFree(devA));
    CUDA_CHECK(cudaFree(devB));
    CUDA_CHECK(cudaFree(devC));

    free(a);
    free(b);
    free(cGpu);
    free(cCpu);
}

int main(void) {
    int choice;

    srand((unsigned int)time(NULL));

    printf("CUDA Practical Program\n");
    printf("1. Vector Addition\n");
    printf("2. Matrix Multiplication\n");
    printf("3. Run Both\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
    case 1:
        runVectorAddition();
        break;
    case 2:
        runMatrixMultiplication();
        break;
    case 3:
        runVectorAddition();
        runMatrixMultiplication();
        break;
    default:
        printf("Invalid choice.\n");
    }

    return 0;
}
