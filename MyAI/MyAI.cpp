#include <iostream>
#include "CNN.h"



int main()
{
	
	myai::cnn::CNN cnn{
		new myai::cnn::Layer(128 * 128),
		new myai::cnn::Layer(1000),
		new myai::cnn::Layer(30),
		new myai::cnn::Layer(1000),
		new myai::cnn::Layer(128 * 128)
	};

	cnn.compute();
	
	std::cin.get();
	
}


