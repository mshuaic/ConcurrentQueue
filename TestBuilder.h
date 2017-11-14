#ifndef TESTBUILDER_H
#define TESTBUILDER_H


#include <random>

class TestBuilder
{
private:
	typedef unsigned int uint;
	uint* randOps;
	uint* randNums;
	uint numOps;
	uint currOp = 0;

public:
	TestBuilder(uint numOps, int  numOfTypes)
	{
		this->numOps = numOps;
		this->randOps = new uint[numOps];
		this->randNums = new uint[numOps];
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dist(1, numOfTypes);
		for (int i = 0; i < numOps; i++)
		{
			randOps[i] = dist(gen);
		}

		for (int i = 0; i < numOps; i++)
		{
			randNums[i] = gen();
		}
	}
	~TestBuilder()
	{
		delete[] randOps;
		delete[] randNums;
	}

	uint getNextOp()
	{
		return randOps[currOp];
	}
	
	uint getNextNum()
	{
		return randNums[currOp];
	}

	void nextOp()
	{
		currOp++;
	}
};


#endif