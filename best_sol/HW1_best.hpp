
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <mutex>

// const int FILTER_SIZE=10; // The FILTER_SIZE is already defined in Makefile

void mac(float* array_out_parallel, const float* k, const float* array_in, int tid, const int N, const int NT) {

	for(int i = tid*(N/NT) - (tid > 0 ? FILTER_SIZE: 0); i < tid*(N/NT) + (N/NT) - FILTER_SIZE + (tid == NT - 1 ? 1 : 0); i++) {

	//	int idx = tid*(N/NT) + i;

//		if(idx <= N - FILTER_SIZE) {
			float  temp = array_out_parallel[i];

			for(int j = 0; j < FILTER_SIZE; j++) {

				temp += array_in[i+j] * k[j];
			}

			array_out_parallel[i] = temp;
//		}
	}

}


inline void parallel_1d_filter(const int N, const int NT, const float* array_in, const float* k, float* array_out_parallel) { 

	std::thread* threads = new std::thread[NT];

	for(int t = 0; t < NT; t++) {
		threads[t] = std::thread(mac, array_out_parallel, k, array_in, t, N, NT);
	}
	for(int t = 0; t < NT; t++) {
		threads[t].join();
	}

	delete[] threads;
}
