#if _MSC_VER
#define  _CRT_SECURE_NO_WARNINGS
#endif

#include <cstdio>
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include "tbb/concurrent_queue.h"
#include "BLOCKING_QUEUE.h"
#include "NONBLOCKING_QUEUE.h"
#include "TestBuilder.h"
#define MAX_NUM_OP 10000
#define CAPACITY 5000
#define MAX_NUM_THREAD 32
#define TOTAL_NUM_RUNS 5
#define ADD_OP_PERCENT 20
#define TOTAL_PERCENTAGE 100

using namespace std;

bool ready = false;
void test_blocking_queue()
{
	TestBuilder tb(MAX_NUM_OP, TOTAL_PERCENTAGE);
	BLOCKING_QUEUE<unsigned int> q(CAPACITY);
	while (!ready)
		std::this_thread::yield();
	for(int i=0;i<MAX_NUM_OP;i++)
	{
		unsigned int num = tb.getNextNum();
		unsigned int op = tb.getNextOp();
		if (op <= ADD_OP_PERCENT)
		{
			q.add(num);
		}
		else
			q.remove(&num);
		tb.nextOp();
	}
}

void test_nonblocking_queue()
{
	TestBuilder tb(MAX_NUM_OP, TOTAL_PERCENTAGE);
	NONBLOCKING_QUEUE<unsigned int> q(CAPACITY);
	while (!ready)
		std::this_thread::yield();
	for (int i = 0; i<MAX_NUM_OP; i++)
	{
		unsigned int num = tb.getNextNum();
		unsigned int op = tb.getNextOp();
		if (op <= ADD_OP_PERCENT)
		{
			q.add(num);
		}
		else
			q.remove(&num);
		tb.nextOp();
	}
}

void test_tbb_queue()
{
	TestBuilder tb(MAX_NUM_OP, TOTAL_PERCENTAGE);
	tbb::concurrent_bounded_queue<unsigned int> q;
	q.set_capacity(CAPACITY);
	while (!ready)
		std::this_thread::yield();
	for (int i = 0; i<MAX_NUM_OP; i++)
	{
		unsigned int num = tb.getNextNum();
		unsigned int op = tb.getNextOp();
		if (op <= ADD_OP_PERCENT)
		{
			q.try_push(num);
		}
		else
			q.try_pop(num);
		tb.nextOp();
	}

}

class Timer
{
public:
	Timer() : beg_(clock_::now()) {}
	void reset() { beg_ = clock_::now(); }
	double elapsed() const {
		return std::chrono::duration_cast<std::chrono::milliseconds>
			(clock_::now() - beg_).count();
	}

private:
	typedef std::chrono::high_resolution_clock clock_;
	typedef std::chrono::duration<double, std::ratio<1> > second_;
	std::chrono::time_point<clock_> beg_;
};


int test_thread(int numOfThreads, std::function<void()> func)
{
	Timer tmr;
	ready = false;
	vector<thread> threads;
	for (int j = 0; j < numOfThreads; j++)
	{
		threads.push_back(thread(func));
	}
	// wait for all threads spawn and allocate test cases
	//this_thread::sleep_for(std::chrono::seconds(1));
	tmr.reset();
	ready = true;
	for (auto &th : threads)
	{
		th.join();
	}
	int t = tmr.elapsed();
	//printf("%d\n", t);
	return t;
}

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void printProgress(double percentage)
{
	int val = (int)(percentage * 100);
	int lpad = (int)(percentage * PBWIDTH);
	int rpad = PBWIDTH - lpad;
	printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
	fflush(stdout);
}


int main()
{	
	int result[3][MAX_NUM_THREAD] = { 0 };
	
	for (int n = 0; n < TOTAL_NUM_RUNS; n++)
	{
		printProgress(0.2*n);
		for (int i = 1; i <= MAX_NUM_THREAD; i++)
		{
			result[0][i - 1] += test_thread(i, test_blocking_queue);
		}
		for (int i = 1; i <= MAX_NUM_THREAD; i++)
		{
			result[1][i - 1] += test_thread(i, test_nonblocking_queue);
		}
		for (int i = 1; i <= MAX_NUM_THREAD; i++)
		{
			result[2][i - 1] += test_thread(i, test_tbb_queue);
		}
		
	}
	printProgress(1);
	FILE* pFile;
	char filename[20];
	std::sprintf(filename, "%dAddOp.txt", ADD_OP_PERCENT);
	pFile = std::fopen(filename, "w");
	//fprintf(pFile, "Add Operation Percentage: %d%%", ADD_OP_PERCENT);
	//fprintf(stdout, "Add Operation Percentage: %d%%", ADD_OP_PERCENT);
	fprintf(pFile, "Threads,  blocking,   nonblocking,  tbb\n");
	printf("\nThreads,  blocking,   nonblocking,  tbb\n");
	for (int i = 0; i < MAX_NUM_THREAD; i++)
	{
		printf("%02d, %8d, %10d, %12d\n",i+1,
			result[0][i]/TOTAL_NUM_RUNS, result[1][i]/TOTAL_NUM_RUNS, result[2][i]/TOTAL_NUM_RUNS);
		fprintf(pFile,"%02d, %8d, %10d, %12d\n", i + 1,
			result[0][i] / TOTAL_NUM_RUNS, result[1][i] / TOTAL_NUM_RUNS, result[2][i] / TOTAL_NUM_RUNS);
	}
	fclose(pFile);
	printf("done\n");
	getchar();
	return 0;
}
