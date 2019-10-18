#include <iostream>
#include "CNN.h"



int main()
{
	myai::cnn::CNN cnn{
		256 * 256,
		1000,
		30,
		1000,
		256 * 256
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
		a = a / v.size();
		std::cout << "Iteration  " << i << " finished in " << average<<" with average " << a << std::endl;
	}
	std::cout << "calculation finished, ";
	std::cout << average;

	return 0;
}


