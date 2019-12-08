#include "CNN.h"

#define EXPORT extern "C" _declspec(dllexport)

using namespace myai::cnn;

EXPORT void* newCNN(void* layers, unsigned int layer_count) {
	return (void*) new CNN((unsigned int*)layers, layer_count);
}

EXPORT void delCNN(void* p) {
	((CNN*)p)->~CNN();
}