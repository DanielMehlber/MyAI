#pragma once
#include <math.h>

#define BIG_ARRAY_SPLIT_SIZE 500
#define COMPUTATION_THREADS 100

#ifdef _DEBUG
#define myai_dlog(x) std::cout << "[MYAI-DEBUG] " << x << std::endl;
#define myai_ldlog(x) std::cout << "[MYAI-DEBUG] " << x;
#define dlog(x) std::cout << x << std::endl;
#define ldlog(x) std::cout << x;
#else
#define myai_dlog(x);
#define myai_ldlog(x);
#define dlog(x);
#define ldlog(x);
#endif

#define VERSION 1.2f

namespace myai {

	class Network {
	public:
		struct SETTINGS {
			unsigned int threaded_neuron_count = 100;
		} settings;
	};

	namespace utils {
		inline float clamp(float c) {
			return tanh(c);
		}
	}

}