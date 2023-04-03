#include <vector>
#include <thread>

void thread_func(int start, int end, float *array_in, const float *k, float *array_out_parallel)
{
	for (int i = start; i < end; ++i)
	{
		for (int j = 0; j < FILTER_SIZE; ++j)
		{
			array_out_parallel[i] += array_in[i + j] * k[j];
		}
	}
}

void parallel_1d_filter(int N, int NT, float *array_in, const float *k, float *array_out_parallel)
{
	std::vector<std::thread> threads;

	int step = N - (FILTER_SIZE - 1) / NT;
	int remainder = N - (FILTER_SIZE - 1) % NT;

	int start = 0;
	for (int t = 0; t < NT; ++t)
	{
		int extra = (t < remainder) ? 1 : 0;
		int end = start + step + extra;
		threads.emplace_back(thread_func, start, end, array_in, k, array_out_parallel);
		start = end;
	}
	for (auto& thread : threads)
	{
		thread.join();
	}
}