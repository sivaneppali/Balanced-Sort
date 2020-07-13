all: programa

programa:
	 g++ -o sort.exe sort.cpp -std=c++11 -pthread
