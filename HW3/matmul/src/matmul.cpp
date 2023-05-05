#include "matmul.h"
#include <thread>
#include <vector>
#include <omp.h>
#include <immintrin.h>

void matmul_thread(const int* const matrixA, const int* const matrixB,
                   int* const matrixC, const int n, const int m,
                   const int startRow, const int endRow) {
  for (int i = startRow; i < endRow; ++i) {
    for (int j = 0; j < n; ++j) {
      int sum = 0;
      for (int k = 0; k < m; ++k) {
        sum += matrixA[i * m + k] * matrixB[k * n + j];
      }
      matrixC[i * n + j] = sum;
    }
  }
}

void transpose(const int* const matrix, int* const transposedMatrix, const int n) {
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      transposedMatrix[j * n + i] = matrix[i * n + j];
    }
  }
}

void matmul_thread_wTranspose(const int* const matrixA, const int* const matrixBT,
                   int* const matrixC, const int n, const int m,
                   const int startRow, const int endRow) {
  for (int i = startRow; i < endRow; ++i) {
    for (int j = 0; j < n; ++j) {
      int sum = 0;
      for (int k = 0; k < m; ++k) {
        sum += matrixA[i * m + k] * matrixBT[j * n + k];
      }
      matrixC[i * n + j] = sum;
    }
  }
}

void matmul_block(const int* const matrixA, const int* const matrixBT,
                   int* const matrixC, const int n, const int m,
                   int threadId, int numThreads, const int blockSize) {
  for (int ii = threadId * blockSize; ii < n; ii += numThreads * blockSize) {
    for (int jj = 0; jj < n; jj += blockSize) {
      for (int kk = 0; kk < m; kk += blockSize) {
        for (int i = ii; i < ii + blockSize; ++i) {
          for (int j = jj; j < jj + blockSize; ++j) {
            int sum = matrixC[i * n + j];
            for (int k = kk; k < kk + blockSize; ++k) {
              sum += matrixA[i * m + k] * matrixBT[j * n + k];
            }
            matrixC[i * n + j] = sum;
          }
        }
      }
    }
  }
}

void matmul_SIMD(const int* const matrixA, const int* const matrixBT,
                   int* const matrixC, const int n, const int m,
                   int threadId, int numThreads, const int blockSize) {
  for (int ii = threadId * blockSize; ii < n; ii += numThreads * blockSize) {
    for (int jj = 0; jj < n; jj += blockSize) {
      for (int kk = 0; kk < m; kk += blockSize) {
        for (int i = ii; i < ii + blockSize; ++i) {
          for (int j = jj; j < jj + blockSize; ++j) {
            __m256i c_vec = _mm256_setzero_si256();
            for (int k = kk; k < kk + blockSize; k += 8) {
              __m256i a_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&matrixA[i * m + k]));
              __m256i bt_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&matrixBT[j * n + k]));
              c_vec = _mm256_add_epi32(c_vec, _mm256_mullo_epi32(a_vec, bt_vec));
            }
            int partials[8];
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(partials), c_vec);
            for (int idx = 0; idx < 8; ++idx) {
              matrixC[i * n + j] += partials[idx];
            }
          }
        }
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


void matmul_ref(const int* const matrixA, const int* const matrixB,
                int* const matrixC, const int n, const int m) {
  // You can assume matrixC is initialized with zero
  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      for (int k = 0; k < m; k++)
        matrixC[i * n + j] += matrixA[i * m + k] * matrixB[k * n + j];
}

void matmul_optimized(const int* const matrixA, const int* const matrixB,
                      int* const matrixC, const int n, const int m) {
  // TODO: Implement your code
  // You can assume matrixC is initialized with zero

  int numThreads = std::thread::hardware_concurrency();
  std::vector<std::thread> threads;

  // Transpose matrixB
  int* matrixBT = new int[m * n];
  transpose(matrixB, matrixBT, m);

  // Block size
  const int blockSize = 64;

//////////////////////////////////////////////////////////////////////

/** Trial 1
 * Row-wise Block Striping
***/
  // int rowsPerThread = n / numThreads;

  // for (int t = 0; t < numThreads; ++t) {
  //   int startRow = t * rowsPerThread;
  //   int endRow = (t == numThreads - 1) ? n : startRow + rowsPerThread;

  //   threads.emplace_back(matmul_thread, matrixA, matrixBT, matrixC, n, m, startRow, endRow);
  // }

/*** Trial 2
 * Divide the work among available threads, assigning a portion of rows in the resulting matrix C to each thread.
 * Transpose matrix B to reduce cache misses
 * Use std::thread for parallelization.
 * Join the threads after the work is done.
***/
  // // Transpose matrixB
  // int* matrixBT = new int[m * n];
  // transpose(matrixB, matrixBT, m);

  // int rowsPerThread = n / numThreads;

  // for (int t = 0; t < numThreads; ++t) {
  //   int startRow = t * rowsPerThread;
  //   int endRow = (t == numThreads - 1) ? n : startRow + rowsPerThread;

  //   threads.emplace_back(matmul_thread_wTranspose, matrixA, matrixBT, matrixC, n, m, startRow, endRow);
  // }

  // // Join threads
  // for (auto& t : threads) {
  //   t.join();
  // }

/*** Trial 3
 * Transpose matrix B to minimize cache misses.
 * Use blocked matrix multiplication to take advantage of cache locality.
***/

  // for (int i = 0; i < numThreads; ++i) {
  //   threads.emplace_back(matmul_block, matrixA, matrixBT, matrixC, n, m, i, numThreads, blockSize);
  // }

  // // Join threads
  // for (auto& t : threads) {
  //   t.join();
  // }

/*** Trial 4
 * Use blocked matrix multiplication to take advantage of cache locality.
 * Transpose matrix B to minimize cache misses.
 * Use OpenMP for parallelization.
 * Apply loop unrolling to further optimize the performance.
***/

  // #pragma omp parallel for
  // for (int ii = 0; ii < n; ii += blockSize) {
  //   for (int jj = 0; jj < n; jj += blockSize) {
  //     for (int kk = 0; kk < m; kk += blockSize) {
  //       for (int i = ii; i < ii + blockSize; ++i) {
  //         for (int j = jj; j < jj + blockSize; ++j) {
  //           int sum = matrixC[i * n + j];
  //           for (int k = kk; k < kk + blockSize; ++k) {
  //             sum += matrixA[i * m + k] * matrixBT[j * n + k];
  //           }
  //           matrixC[i * n + j] = sum;
  //         }
  //       }
  //     }
  //   }
  // }

/*** Trial 5
 * Inte's SSE (Streaming SIMD Extensions) intrinsics
***/
  for (int i = 0; i < numThreads; ++i) {
    threads.emplace_back(matmul_SIMD, matrixA, matrixBT, matrixC, n, m, i, numThreads, blockSize);
  }

  // Join threads
  for (auto& t : threads) {
    t.join();
  }

/*** Trial 6
 * SIMD with OpenMP
***/

  // #pragma omp parallel for
  // for (int ii = 0; ii < n; ii += blockSize) {
  //   for (int jj = 0; jj < n; jj += blockSize) {
  //     for (int kk = 0; kk < m; kk += blockSize) {
  //       for (int i = ii; i < std::min(ii + blockSize, n); ++i) {
  //         for (int j = jj; j < std::min(jj + blockSize, n); ++j) {
  //           __m256i c_vec = _mm256_setzero_si256();
  //           for (int k = kk; k < std::min(kk + blockSize, m); k += 8) {
  //             __m256i a_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&matrixA[i * m + k]));
  //             __m256i b_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&matrixBT[j * m + k]));
  //             c_vec = _mm256_add_epi32(c_vec, _mm256_mullo_epi32(a_vec, b_vec));
  //           }
  //           int partials[8];
  //           _mm256_storeu_si256(reinterpret_cast<__m256i*>(partials), c_vec);
  //           for (int idx = 0; idx < 8; ++idx) {
  //             matrixC[i * n + j] += partials[idx];
  //           }
  //         }
  //       }
  //     }
  //   }
  // }

//////////////////////////////////////////////////////////////////////

  delete[] matrixBT;

}