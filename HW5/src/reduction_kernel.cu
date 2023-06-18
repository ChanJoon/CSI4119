#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <iostream>

#include "reduction.h"


int get_max_threads()
{
    int dev_num;
    int max_threads;
    cudaError_t status;

    // Grab the device number of the default CUDA device.
    status = cudaGetDevice(&dev_num);
    assert(status==cudaSuccess);

    // Query the max possible number of threads per block
    status = cudaDeviceGetAttribute(&max_threads, cudaDevAttrMaxThreadsPerBlock, dev_num);
    assert(status==cudaSuccess);

    return max_threads;
}

void allocateDeviceMemory(void** M, int size)
{
    cudaError_t err = cudaMalloc(M, size);
    assert(err==cudaSuccess);
}


void deallocateDeviceMemory(void* M)
{
    cudaError_t err = cudaFree(M);
    assert(err==cudaSuccess);
}

void cudaMemcpyToDevice(void* dst, void* src, int size) {
    cudaError_t err = cudaMemcpy((void*)dst, (void*)src, size, cudaMemcpyHostToDevice);
    assert(err==cudaSuccess);
}

void cudaMemcpyToHost(void* dst, void* src, int size) {
    cudaError_t err = cudaMemcpy((void*)dst, (void*)src, size, cudaMemcpyDeviceToHost);
    assert(err==cudaSuccess);
}

void reduce_ref(const int* const g_idata, int* const g_odata, const int n) {
    for (int i = 0; i < n; i++)
        g_odata[0] += g_idata[i];

    printf("CPU result: %d \n", g_odata[0]);
}

__global__ void reduce1(int *g_idata, int *g_odata, unsigned int n);
__global__ void reduce2(int *g_idata, int *g_odata, unsigned int n);
__global__ void reduce3(int *g_idata, int *g_odata, unsigned int n);
__global__ void reduce4(int *g_idata, int *g_odata, unsigned int n);
__global__ void reduce5(int *g_idata, int *g_odata, unsigned int n);
template <const int blockSize> __global__ void reduce6(int *g_idata, int *g_odata, unsigned int n);
template <const int blockSize> __global__ void reduce7(int *g_idata, int *g_odata, unsigned int n);

void reduce_optimize(const int* const g_idata, int* const g_odata, const int* const d_idata, int* const d_odata, const int n) {
    // TODO: Implement your CUDA code
    // Reduction result must be stored in d_odata[0] 
    // You should run the best kernel in here but you must remain other kernels as evidence.

    const int maxThreads = get_max_threads();

    int threads = (n < maxThreads) ? n : maxThreads;
    int blocks = (n + threads - 1) / threads;

    int remain = blocks / threads;

/**
 * @kernel 1 to 3
 */
    // reduce1<<<blocks, threads, threads*sizeof(int)>>>((int *)d_idata, d_odata, n);
    // reduce1<<<remain, threads, threads*sizeof(int)>>>((int *)d_odata, d_odata, blocks);
    // reduce1<<<1, remain, remain*sizeof(int)>>>((int *)d_odata, d_odata, remain);

    // reduce2<<<blocks, threads, threads*sizeof(int)>>>((int *)d_idata, d_odata, n);
    // reduce2<<<remain, threads, threads*sizeof(int)>>>((int *)d_odata, d_odata, blocks);
    // reduce2<<<1, remain, remain*sizeof(int)>>>((int *)d_odata, d_odata, remain);

    // reduce3<<<blocks, threads, threads*sizeof(int)>>>((int *)d_idata, d_odata, n);
    // reduce3<<<remain, threads, threads*sizeof(int)>>>((int *)d_odata, d_odata, blocks);
    // reduce3<<<1, remain, remain*sizeof(int)>>>((int *)d_odata, d_odata, remain);

/**
 * @kernel 4 - 5
 */
    // reduce4<<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
    // reduce4<<<2*remain, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, blocks);
    // reduce4<<<1, remain, remain*sizeof(int)>>>((int *)d_odata, d_odata, 2*remain);

    // reduce5<<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
    // reduce5<<<2*remain, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, blocks);
    // reduce5<<<1, remain, remain*sizeof(int)>>>((int *)d_odata, d_odata, 2*remain);

/**
 * @kernel 6 - 7
 * manually adjust blocks and threads for remaining partial sums
 * For partial sum reduction, Invoke kernel multiple times
 */
    // for(int i=0; i<3; ++i){
    //     if(i==0) {
    //         switch(threads/2) {
    //             case 512:
    //                 reduce6<512><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
    //                 break;
    //             case 256:
    //                 reduce6<256><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
    //                 break;
    //             case 128:
    //                 reduce6<128><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
    //                 break;
    //             case 64:
    //                 reduce6<64><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
    //                 break;
    //             case 32:
    //                 reduce6<32><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
    //                 break;
    //             case 16:
    //                 reduce6<16><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
    //                 break;
    //             case 8:
    //                 reduce6<8><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
    //                 break;
    //             case 4:
    //                 reduce6<4><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
    //                 break;
    //             case 2:
    //                 reduce6<2><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
    //                 break;
    //             case 1:
    //                 reduce6<1><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
    //                 break;
    //         }
    //         const_cast<int &> (n) = blocks; // 4096
    //         blocks = 2*remain; // 8
    //     }
    //     else {
    //         switch(threads/2) {
    //             case 512:
    //                 reduce6<512><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
    //                 break;
    //             case 256:
    //                 reduce6<256><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
    //                 break;
    //             case 128:
    //                 reduce6<128><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
    //                 break;
    //             case 64:
    //                 reduce6<64><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
    //                 break;
    //             case 32:
    //                 reduce6<32><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
    //                 break;
    //             case 16:
    //                 reduce6<16><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
    //                 break;
    //             case 8:
    //                 reduce6<8><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
    //                 break;
    //             case 4:
    //                 reduce6<4><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
    //                 break;
    //             case 2:
    //                 reduce6<2><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
    //                 break;
    //             case 1:
    //                 reduce6<1><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
    //                 break;
    //         }
    //         if (i==1) {
    //             const_cast<int &> (n) = blocks; // 8
    //             threads = blocks; // 8
    //             blocks = 1;
    //         }
    //     }
    // }

// kenel 7
    switch(threads/2) {
        case 512:
            reduce7<512><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
            break;
        case 256:
            reduce7<256><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
            break;
        case 128:
            reduce7<128><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
            break;
        case 64:
            reduce7<64><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
            break;
        case 32:
            reduce7<32><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
            break;
        case 16:
            reduce7<16><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
            break;
        case 8:
            reduce7<8><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
            break;
        case 4:
            reduce7<4><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
            break;
        case 2:
            reduce7<2><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
            break;
        case 1:
            reduce7<1><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_idata, d_odata, n);
            break;
    }
    const_cast<int &> (n) = blocks; // 4096
    blocks = 2*remain; // 8

    switch(threads/2) {
        case 512:
            reduce7<512><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 256:
            reduce7<256><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 128:
            reduce7<128><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 64:
            reduce7<64><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 32:
            reduce7<32><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 16:
            reduce7<16><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 8:
            reduce7<8><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 4:
            reduce7<4><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 2:
            reduce7<2><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 1:
            reduce7<1><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
    }
    const_cast<int &> (n) = blocks; // 8
    threads = blocks; // 8
    blocks = 1;
    
    switch(threads/2) {
        case 512:
            reduce7<512><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 256:
            reduce7<256><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 128:
            reduce7<128><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 64:
            reduce7<64><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 32:
            reduce7<32><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 16:
            reduce7<16><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 8:
            reduce7<8><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 4:
            reduce7<4><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 2:
            reduce7<2><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
        case 1:
            reduce7<1><<<blocks, threads/2, threads/2*sizeof(int)>>>((int *)d_odata, d_odata, n);
            break;
    }
}

// Interleaved Addressing with divergent branches
__global__ void reduce1(int *g_idata, int *g_odata, unsigned int n) {
    extern __shared__ int sdata[];

    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;

    sdata[tid] = (i < n) ? g_idata[i] : 0;
    __syncthreads();

    for (unsigned int s = 1; s < blockDim.x; s *= 2) {
        if (tid % (2 * s) == 0) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    if (tid == 0) {
        g_odata[blockIdx.x] = sdata[0];
    }
}

// Interleaved Addressing with Bank Conflicts
__global__ void reduce2(int *g_idata, int *g_odata, unsigned int n) {
    extern __shared__ int sdata[];

    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;

    sdata[tid] = (i < n) ? g_idata[i] : 0;
    __syncthreads();

    for (unsigned int s = 1; s < blockDim.x; s *= 2) {
        int index = 2 * s * tid;
        if (index < blockDim.x) {
            sdata[index] += sdata[index + s];
        }
        __syncthreads();
    }

    if (tid == 0) {
        g_odata[blockIdx.x] = sdata[0];
    }
}

// Sequential Addressing
__global__ void reduce3(int *g_idata, int *g_odata, unsigned int n) {
    extern __shared__ int sdata[];

    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;

    sdata[tid] = (i < n) ? g_idata[i] : 0;
    __syncthreads();

    for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    if (tid == 0) {
        g_odata[blockIdx.x] = sdata[0];
    }
}

// First Add during Global Memory Load
__global__ void reduce4(int* g_idata, int* g_odata, unsigned int n) {
    extern __shared__ int sdata[];

    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * (blockDim.x * 2) + threadIdx.x;

    int temp = (i < n) ? g_idata[i] : 0;
    temp += g_idata[i + blockDim.x];
    sdata[tid] = temp;
    __syncthreads();

    for (unsigned int s = blockDim.x/2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    if (tid == 0) {
        g_odata[blockIdx.x] = sdata[0];
    }
}

// Unrolling Last Warp
__global__ void reduce5(int* g_idata, int* g_odata, unsigned int n) {
    extern __shared__ int sdata[];

    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * (blockDim.x * 2) + threadIdx.x;

    int temp = (i < n) ? g_idata[i] : 0;
    temp += g_idata[i + blockDim.x];
    sdata[tid] = temp;
    __syncthreads();

    for (unsigned int s = blockDim.x/2; s > 32; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    if (tid < 32) {
        volatile int * smem = sdata;
        if (blockDim.x >=  64) smem[tid] += smem[tid + 32];
        if (blockDim.x >=  32) smem[tid] += smem[tid + 16];
        if (blockDim.x >=  16) smem[tid] += smem[tid + 8];
        if (blockDim.x >=  8) smem[tid] += smem[tid + 4];
        if (blockDim.x >=  4) smem[tid] += smem[tid + 2];
        if (blockDim.x >=  2) smem[tid] += smem[tid + 1];
    }

    if (tid == 0) {
        g_odata[blockIdx.x] = sdata[0];
    }
}

// Completely Loop Unrolling
template <const int blockSize>
__global__ void reduce6(int* g_idata, int* g_odata, unsigned int n) {
    extern __shared__ int sdata[];

    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * (blockSize * 2) + threadIdx.x;

    int temp = (i < n) ? g_idata[i] : 0;
    temp += g_idata[i + blockSize];
    sdata[tid] = temp;
    __syncthreads();

    if (blockSize >= 512) { if (tid < 256) { sdata[tid] += sdata[tid + 256]; } __syncthreads(); }
    if (blockSize >= 256) { if (tid < 128) { sdata[tid] += sdata[tid + 128]; } __syncthreads(); }
    if (blockSize >= 128) { if (tid < 64) { sdata[tid] += sdata[tid + 64]; } __syncthreads(); }

    if (tid < 32) {
        volatile int * smem = sdata;
        if (blockSize >= 64) smem[tid] += smem[tid + 32];
        if (blockSize >= 32) smem[tid] += smem[tid + 16];
        if (blockSize >= 16) smem[tid] += smem[tid + 8];
        if (blockSize >= 8) smem[tid] += smem[tid + 4];
        if (blockSize >= 4) smem[tid] += smem[tid + 2];
        if (blockSize >= 2) smem[tid] += smem[tid + 1];
    }
    if (tid == 0) {
        g_odata[blockIdx.x] = sdata[0];
    }
}

// Multiple elements per Thread
template <const int blockSize>
__global__ void reduce7(int *g_idata, int *g_odata, unsigned int n)
{
    extern __shared__ int sdata[];

    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x*(blockSize*2) + tid;
    unsigned int gridSize = blockSize*2*gridDim.x;
    sdata[tid] = 0;

    while (i < n) { sdata[tid] += g_idata[i] + g_idata[i+blockSize]; i += gridSize; }
    __syncthreads();

    if (blockSize >= 512) { if (tid < 256) { sdata[tid] += sdata[tid + 256]; } __syncthreads(); }
    if (blockSize >= 256) { if (tid < 128) { sdata[tid] += sdata[tid + 128]; } __syncthreads(); }
    if (blockSize >= 128) { if (tid < 64) { sdata[tid] += sdata[tid + 64]; } __syncthreads(); }

    if (tid < 32) {
        volatile int * smem = sdata;
        if (blockSize >= 64) smem[tid] += smem[tid + 32];
        if (blockSize >= 32) smem[tid] += smem[tid + 16];
        if (blockSize >= 16) smem[tid] += smem[tid + 8];
        if (blockSize >= 8) smem[tid] += smem[tid + 4];
        if (blockSize >= 4) smem[tid] += smem[tid + 2];
        if (blockSize >= 2) smem[tid] += smem[tid + 1];
    }
    if (tid == 0) g_odata[blockIdx.x] = sdata[0];
}