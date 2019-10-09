#include <iostream>
#include "CNN.h"



int main()
{
	/*
	myai::cnn::CNN cnn{
		new myai::cnn::Layer(128 * 128),
		new myai::cnn::Layer(1000),
		new myai::cnn::Layer(30),
		new myai::cnn::Layer(1000),
		new myai::cnn::Layer(128 * 128)
	};

	cnn.compute();
	*/

	unsigned int size = 256 * 256;
	myai::types::big_array<float> arr(size, 5000);
	//std::vector<float> varr; varr.resize(size);
	std::cout << std::endl;
}


