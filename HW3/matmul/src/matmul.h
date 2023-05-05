#ifndef MATMUL_H
#define MATMUL_H

#include <algorithm>

void matmul_ref(const int* const matrixA, const int* const matrixB,
                int* const matrixC, const int n, const int m);
void matmul_optimized(const int* const matrixA, const int* const matrixB,
                      int* const matrixC, const int n, const int m);
void transpose(const int* const matrix, int* const transposedMatrix, const int n);
void matmul_block(const int* const matrixA, const int* const matrixBT,
                   int* const matrixC, const int n, const int m,
                   int threadId, int numThreads, const int blockSize);
void matmul_SIMD(const int* const matrixA, const int* const matrixBT,
                   int* const matrixC, const int n, const int m,
                   int threadId, int numThreads, const int blockSize);

#endif