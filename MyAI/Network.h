#pragma once
#include <math.h>

#define BIG_ARRAY_SPLIT_SIZE 500
#define COMPUTATION_THREADS 100

namespace myai {

	class Network {

	};

	namespace utils {
		inline float clamp(float c) {
			return tanh(c);
		}
	}

}