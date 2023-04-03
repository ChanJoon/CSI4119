#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

#include "HW1.hpp"

int main() 
{
  const int N = 1073741824; // num_items
  const int NT = 16; // the Number of Threads
  // const int FILTER_SIZE = 10; // The FILTER_SIZE is already defined in Makefile

  //0. Initialize

  const float k[FILTER_SIZE] = {1.23, 4.56, 7.89, 0.12, 3.45, 6.78, 9.01, 2.34, 5.67, 8.90};
  float *array_in = new float[N];
  float *array_out_serial = new float[N];
  float *array_out_parallel = new float[N];
  {
    std::chrono::duration<double> diff;
    auto start = std::chrono::steady_clock::now();
    for(int i=0;i<N;i++) {
      array_in[i] = i;
    }
    auto end = std::chrono::steady_clock::now();
    diff = end-start;
    std::cout << "init took " << std::fixed << diff.count() << " sec" << std::endl;
  }

  {
    //1. Serial
    std::chrono::duration<double> diff;
    for (int b=0;b<10;b++) { 
      auto start = std::chrono::steady_clock::now();
      for(int i=0;i<N-(FILTER_SIZE-1);i++) {
        for(int j=0;j<FILTER_SIZE;j++) {
          array_out_serial[i] += array_in[i+j] * k[j];
        }
      }
      auto end = std::chrono::steady_clock::now();
      diff += (end-start);
    }
    diff /= 10;
    std::cout << "serial 1D filter took "<< std::fixed << diff.count() << " sec" << std::endl;
  }

  {
    //2. parallel 1D filter
    std::chrono::duration<double> diff;
    for (int b=0;b<10;b++) { 
    auto start = std::chrono::steady_clock::now();

    parallel_1d_filter(N, NT, array_in, k, array_out_parallel);

    auto end = std::chrono::steady_clock::now();
    diff += (end-start);
    }
    diff /= 10;
    std::cout<< std::fixed <<"parallel 1D filter took " << std::fixed << diff.count() << " sec" << std::endl;

    int error_counts=0;
    const float epsilon = 0.01;
    for(int i=0;i<N;i++) {
      float err= std::abs(array_out_serial[i] - array_out_parallel[i]);
      if(err > epsilon) {
        error_counts++;
        if(error_counts < 5) {
          std::cout<<"ERROR at "<<i<<": Serial["<<i<<"] = "<<array_out_serial[i]<<" Parallel["<<i<<"] = "<<array_out_parallel[i]<<std::endl;
          std::cout<<"err: "<<err<<std::endl;
        }
      }
    }

    if(error_counts==0) {
      std::cout<<"PASS"<<std::endl;
    } else {
      std::cout<<"There are "<<error_counts<<" errors"<<std::endl;
    }

  }

  delete[] array_in;
  delete[] array_out_serial;
  delete[] array_out_parallel;

  return 0;
}