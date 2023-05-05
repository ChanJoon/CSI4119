#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>

// const int FILTER_SIZE=10; // The FILTER_SIZE is already defined in Makefile

/* TODO: put your own thread function here for parallel processing */
/****************/
void thread_function(const int start, const int end, const float* array_in, const float* k, float* array_out_parallel) {
    for(int i=start;i<end;i++) {
        for(int j=0; j<FILTER_SIZE;j++) {
            array_out_parallel[i] += array_in[i+j] * k[j];
        }
    }
    return;
}
/****************/
/* TODO: put your own thread function here for parallel processing */

inline void parallel_1d_filter(const int N, const int NT, const float* array_in, const float* k, float* array_out_parallel) { 
    /* TODO: put your own parallelized 1D filter here */
    /****************/
    std::thread threads[NT];
    int out_size = N-(FILTER_SIZE-1);
    int chunk = out_size/NT;
    for (int i=0; i<NT; i++) {
        int start = chunk*i;
        int end = start + chunk;
        if (i == NT - 1) end = out_size;
        threads[i] = std::thread([start, end, array_in, k, array_out_parallel]() { thread_function(start, end, array_in, k, array_out_parallel);});
    }
    for (int i=0; i<NT; i++)
    {
        threads[i].join();
    }
    return;
    /****************/
    /* TODO: put your own parallelized 1D filter here */
}