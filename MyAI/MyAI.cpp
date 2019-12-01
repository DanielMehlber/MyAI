#include <iostream>
#include "CNN.h"

#define EXPORT _declspec(dllexport)
#define println(x) std::cout << x << std::endl

int main()
{

	print("Started...");
	myai::cnn::CNN cnn{
		10,
		5,
		3,
		5,
		10
	};

	cnn.save("C://Users/danie/Documents/NN.cnn");
	cnn.load("C://Users/danie/Documents/NN.cnn");

	return 0;
}

