#ifndef NONBLOCKINGQUEUE_H
#define NONBLOCKINGQUEUE_H

#include <atomic>
#include <thread>

// Array based Bounded Total Queue
template <typename T>
class NONBLOCKING_QUEUE
{
private:
	typedef unsigned int uint;
	T* storage;
	uint capacity;
	std::atomic<uint> head;
	std::atomic<uint> tail;
	std::atomic<uint> tailCommit;

	inline int getIndex(int position)
	{
		return position % capacity;
	}
public:
	NONBLOCKING_QUEUE(uint _capacity)
	{
		capacity = _capacity+1;
		storage = new T[capacity];
		head = 0;
		tail = 0;
		tailCommit = 0;
	}
	~NONBLOCKING_QUEUE()
	{
		delete[] storage;
	}

	void add(T item)
	{
		uint currTail; //= tail;
		uint currHead;// = head;
		while (true)
		{
			currTail = tail;
			currHead = head;
			// queue is full
			if (getIndex(currTail + 1) == currHead)
				return;
			// reserve one slot for insertion
			if (tail.compare_exchange_strong(currTail, getIndex(tail + 1)))
			{
				storage[currTail] = item;

				// tail and head only logically increase.
				// The first if statement in add function ensures that tail cannot surpass head.
				// The first if statement in remove function ensures head cannot surpass tailCommit, 
				// so currTail will never circuital reset(because of the getIndex function) and catch up tailCommit.
				while (!tailCommit.compare_exchange_weak(currTail, getIndex(tailCommit + 1))) 
				{
					std::this_thread::yield();
				}
				return;
			}			
		}
	}

	void remove(T* result)
	{
		uint currTailCommit;
		uint currTail;
		uint currHead;
		while (true)
		{
			currTailCommit = tailCommit;
			currTail = tail;
			currHead = head;
			// queue is empty
			if (currHead == currTailCommit)
				return;
			*result = storage[currHead];
			if (head.compare_exchange_strong(currHead, getIndex(head + 1)))
				return;
		}
	}
};

#endif 