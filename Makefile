all: main.cpp
	g++ -std=c++11 -o main main.cpp NONBLOCKING_QUEUE.h BLOCKING_QUEUE.h TestBuilder.h -ltbb -lpthread