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


	std::cout << "Network created\n";
	int threads = 0;
	int reps = 100;
	std::vector<double> v; v.reserve(reps);

	myai::time::clock<> c;
	cnn.compute(threads);
	double average = c.stop();

	for (int i = 0; i < reps; i++) {
		myai::time::clock<> c;
		cnn.compute(threads);
		average = c.stop();
		v.push_back(average);
		double a = 0;
		for (double d : v)
			a += d;
		average = a / v.size();
		std::cout << "Iteration  " << i << " finished with average " << average << std::endl;
	}
	std::cout << "calculation finished, ";
	std::cout << average;

	return 0;
}


