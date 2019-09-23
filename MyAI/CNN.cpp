#include "CNN.h"


myai::cnn::Neuron::Neuron()
{
}

myai::cnn::Neuron::Neuron(Layer* prev) : prev{prev}
{
	weights = big_array<float>(prev->count(), BIG_ARRAY_SPLIT_SIZE);
}

myai::cnn::Neuron::~Neuron()
{
	weights.~big_array();
}

void myai::cnn::Neuron::compute()
{
	if (!prev) //No previos layer to compute
		return;

	unsigned int size = prev->count();
	activation = 0;
	for (unsigned int i = 0; i < size; i++) {
		activation += prev->operator[](i).activation* weights[i];
	}
	activation += bias;
	activation = utils::clamp(activation);
}

myai::cnn::Layer::Layer()
{
}

myai::cnn::Layer::Layer(unsigned int size)
{
	neurons = big_array<Neuron>(size, BIG_ARRAY_SPLIT_SIZE);
}

myai::cnn::Layer::~Layer()
{
	neurons.~big_array();
}

void myai::cnn::Layer::compute()
{
	unsigned int ncount = count();
	processmgr mgr = processmgr(500);
	mgr.process();
	for (unsigned int i = 0; i < ncount; i++) {
		mgr += [=]() {this->operator[](i).compute(); };
	}
	mgr.finish();
}

myai::cnn::CNN::CNN(std::initializer_list<Layer*> t)
{
	Layer* prev = nullptr;
	for (Layer* l : t) {
		for (unsigned int i = 0; i < l->count(); i++) {
			Neuron& n = l->operator[](i);
			n.prev = prev;
			if (prev)
				n.weights = myai::types::big_array<float>(prev->count(), BIG_ARRAY_SPLIT_SIZE);
			prev = l;
		}
		layers.push_back(l);
	}
}


