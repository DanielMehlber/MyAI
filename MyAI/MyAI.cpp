#include <iostream>
#include "CNN.h"

#define EXPORT _declspec(dllexport)
#define println(x) std::cout << x << std::endl

int main()
{

	print("Started...");
	myai::cnn::CNN cnn{
		512 * 512,
		1000,
		30,
		1000,
		512 * 512
	};

	cnn.save("C://Users/danie/Documents/NN.cnn");
	//cnn.load("C://Users/danie/Documents/NN.cnn");

	return 0;
}

