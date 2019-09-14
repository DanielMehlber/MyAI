#include "CNN.h"

void cnn::Neuron::calc()
{
	if (!prev)
		return;
	unsigned int size = prev->count();
	activation = 0;
	for (unsigned int i = 0; i < size; i++) {
		activation += prev->operator[](i).activation* weights[i];
	}
	activation += bias;
	activation = utils::clamp(activation);
}

