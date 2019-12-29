#include <iostream>
#include "CNN.h"

#define EXPORT _declspec(dllexport)
#define println(x) std::cout << x << std::endl

int main()
{

	print("Started...");
	myai::cnn::CNN cnn{
		126,
		1000,
		3,
		1000,
		126
	};

	cnn.compute(AUTO_THREAD_COUNT);
	try {
		cnn.save("test.deleteme");
		cnn.load("test.deleteme");
	}
	catch (myai::types::exception& e) {
		e.pack_info(__FUNCTION__, "Reading or Writing failed.");
		e.printLog();
	}
	return 0;
}

