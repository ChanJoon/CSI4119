#include <stdio.h>
#include <iostream>
#include <chrono>
#include <assert.h>
#include "matmul.h"
using namespace std;


void allocateDeviceMemory(void **M, int size)
{
  cudaError_t err = cudaMalloc(M, size);
  assert(err == cudaSuccess);
}

void deallocateDeviceMemory(void *M)
{
  cudaError_t err = cudaFree(M);
  assert(err == cudaSuccess);
}

void matmul_ref(const int *const matrixA, const int *const matrixB,
                int *const matrixC, const int n)
{
  // You can assume matrixC is initialized with zero
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      for (int k = 0; k < n; k++)
        matrixC[i * n + j] += matrixA[i * n + k] * matrixB[k * n + j];
}

__global__ void MatrixMulKernel(const int* d_A, const int* d_B, int* d_C, const int n);

void matmul_optimized(const int *const matrixA, const int *const matrixB,
                      int *const matrixC, const int *d_A, const int *d_B, int *const d_C, const int n)
{

  // TODO: Implement your CUDA code
  cudaMemcpy((int *)d_A, (const int*)matrixA, n*n*sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy((int *)d_B, (const int*)matrixB, n*n*sizeof(int), cudaMemcpyHostToDevice);

  const int TILE_WIDTH = 32;
  dim3 dimBlock(TILE_WIDTH, TILE_WIDTH, 1);
  dim3 dimGrid(n / TILE_WIDTH, n / TILE_WIDTH, 1);

  MatrixMulKernel<<<dimGrid, dimBlock>>>(d_A, d_B, d_C, n);
  cudaMemcpy((int *)matrixC, (const int*)d_C, n*n*sizeof(int), cudaMemcpyDeviceToHost);
}

__global__ void MatrixMulKernel(const int* d_A, const int* d_B, int* d_C, const int n)
{
  /**
    * Trial 1: Naive version 
    */
  // int row = blockIdx.y * blockDim.y + threadIdx.y;
  // int col = blockIdx.x * blockDim.x + threadIdx.x;

  // float temp = 0;
  // for (int k = 0; k < n; ++k){
  //   d_C[row * n + col] += d_A[row * n + k] * d_B[k * n + col];
  // }

  
  /**
    * Trial 2: Local variables on register
    */
  // int row = blockIdx.y * blockDim.y + threadIdx.y;
  // int col = blockIdx.x * blockDim.x + threadIdx.x;

  // float temp = 0;
  // for (int k = 0; k < n; ++k){
  //   temp += d_A[row * n + k] * d_B[k * n + col];
  // }
  // d_C[row * n + col] = temp;

  /**
    * Trial 3: Blocked matmul with shared memory
    */
  // const int TILE_WIDTH = 32;
  // __shared__ int subTileM[TILE_WIDTH][TILE_WIDTH];
  // __shared__ int subTileN[TILE_WIDTH][TILE_WIDTH];

  // int bx = blockIdx.x; int by = blockIdx.y;
  // int tx = threadIdx.x; int ty = threadIdx.y;

  // int Row = by * TILE_WIDTH + ty;
  // int Col = bx * TILE_WIDTH + tx;
  // float value = 0;

  // for(int m = 0; m < n/TILE_WIDTH; ++m){
  //   subTileM[ty][tx] = d_A[Row*n + m*TILE_WIDTH + tx];
  //   subTileN[ty][tx] = d_B[(m*TILE_WIDTH+ty)*n + Col];
  //   __syncthreads();

  //   for(int k = 0; k < TILE_WIDTH; ++k)
  //     value += subTileM[ty][k] * subTileN[k][tx];
  //   __syncthreads();
  // }
  // d_C[Row*n + Col] = value;
  /**
    * Trial 4: Boundary handling with Blocked matmul
    */
  const int TILE_WIDTH = 32;
  __shared__ int subTileM[TILE_WIDTH][TILE_WIDTH];
  __shared__ int subTileN[TILE_WIDTH][TILE_WIDTH];

  int bx = blockIdx.x; int by = blockIdx.y;
  int tx = threadIdx.x; int ty = threadIdx.y;

  int Row = by * TILE_WIDTH + ty;
  int Col = bx * TILE_WIDTH + tx;
  float value = 0;

  for(int m = 0; m < (n - 1) / TILE_WIDTH + 1; ++m){
    if (Row < n && m * TILE_WIDTH + tx < n)
      subTileM[ty][tx] = d_A[Row * n + m * TILE_WIDTH + tx];
    else 
      subTileM[ty][tx] = 0;

    if (Col < n && m * TILE_WIDTH + ty < n)
      subTileN[ty][tx] = d_B[(m * TILE_WIDTH + ty) * n + Col];
    else 
      subTileM[ty][tx] = 0;
    __syncthreads();

    for(int k = 0; k < TILE_WIDTH; ++k)
      value += subTileM[ty][k] * subTileN[k][tx];
    __syncthreads();
  }
  if (Row < n && Col < n)
    d_C[Row * n + Col] = value;  
}
