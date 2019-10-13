#include <iostream>
#include "CNN.h"



int main()
{
	myai::cnn::CNN cnn{
		128 * 128,
		1000,
		30,
		1000,
		128 * 128
	};

	cnn.compute(0);

	return 0;
}


