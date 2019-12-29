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

	cnn.compute(AUTO_THREAD_COUNT);
	try {
		cnn.save("C://Users/danie/Documents/NN.cnn");
		cnn.load("C://Users/danie/Documents/NN.cnn");
	}
	catch (myai::types::exception& e) {
		e.pack_info(__FUNCTION__, "Cannot read or write.");
		e.printLog();
	}
	return 0;
}

