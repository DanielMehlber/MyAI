#pragma once
#include <math.h>

#define BIG_ARRAY_SPLIT_SIZE 500
#define COMPUTATION_THREADS 100

#ifdef _DEBUG
#define myai_dlog(x) std::cout << "[MYAI-DEBUG] " << x << std::endl;
#else
#define myai_dlog(x) ;
#endif

#define VERSION 1.1f

namespace myai {

	class Network {
	};

	namespace utils {
		inline float clamp(float c) {
			return tanh(c);
		}
	}

}