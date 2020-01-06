#include <iostream>
#include "CNN.h"

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
		cnn.save("test.del");
		cnn.load("test.del");
	}
	catch (myai::types::exception& e) {
		e.pack_info(__FUNCTION__, "Reading or Writing failed.");
		e.printLog();
	}
	return 0;
}

