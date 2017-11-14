#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include <mutex>
#include <atomic>

// Array based Bounded Total Queue
template<typename T>
class BLOCKING_QUEUE
{
private:
	typedef unsigned int uint;
	T* storage;
	uint capacity;
	uint head;
	uint tail;
	std::mutex enqLock;
	std::mutex deqLock;
public:
	BLOCKING_QUEUE(uint _capacity)
	{
		capacity = _capacity+1;
		storage = new T[capacity];
		head = 0;
		tail = 0;
	}
	~BLOCKING_QUEUE()
	{
		delete[] storage;
	}

	void add(T item)
	{
		// queue is full
		std::lock_guard<std::mutex> lock(enqLock);
		if (getIndex(tail + 1) == head)
			return;	
		storage[tail] = item;
		tail = getIndex(tail + 1);
	}

	void remove(T* result) {
		// queue is empty
		std::lock_guard<std::mutex> lock(deqLock);
		if (head == tail)
			return;	
		*result = storage[head];
		head = getIndex(head + 1);
	}
private:
	inline uint getIndex(uint position)
	{
		return position % capacity;
	}
};

#endif