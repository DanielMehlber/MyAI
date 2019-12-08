#include <iostream>
#include "CNN.h"

#define EXPORT _declspec(dllexport)
#define println(x) std::cout << x << std::endl

int main()
{

	print("Started...");
	myai::cnn::CNN cnn{
		126 * 126,
		1000,
		3,
		1000,
		126 * 126
	};

	try {
		cnn.save("C://Users/danie/Documents/NN.cnn");
		cnn.load("C://Users/danie/Documents/NN.cnn");
	}
	catch (std::exception & e) {
		println("ERROR occured: " << e.what());
	}
	return 0;
}

