#include "CNN.h"



myai::cnn::Neuron::Neuron(Layer* owner) : prev{owner}
{
	if(prev)
		weights.resize(owner->count);
}

myai::cnn::Neuron::~Neuron()
{
	weights.~vector();
}

void myai::cnn::Neuron::compute()
{
	if (!prev) //No previos layer to compute
		return;

	unsigned int size = prev->count;
	activation = 0;
	for (unsigned int i = 0; i < size; i++) {
		activation += prev->operator[](i).activation * weights[i];
	}
	activation += bias;
	activation = utils::clamp(activation);
}


myai::cnn::Layer::Layer(unsigned int size, Layer* prev) : count{size}
{
	neurons.reserve(size);
	for (unsigned int i = 0; i < size; i++)
		neurons.push_back(Neuron(prev));
}

myai::cnn::Layer::~Layer()
{
	neurons.~vector();
}

void myai::cnn::Layer::compute()
{
	for (Neuron& n : neurons)
		n.compute();
}

myai::cnn::CNN::CNN(std::initializer_list<int> t)
{
	Layer* prev = nullptr;
	for (int i : t) {
		Layer l(i, prev);
		layers.push_back(l);
		prev = &l;
	}
}

void myai::cnn::CNN::compute()
{
	unsigned int layer_count = layers.size();
	for (unsigned int i = 0; i < layer_count; i++) {
		layers[i].compute();
	}
}

void myai::cnn::CNN::compute(unsigned int thread_count)
{
	if (thread_count == 1) {
		compute();
		return;
	}
	for (Layer& l : layers) {
		l.compute(thread_count);
	}
}

void myai::cnn::Layer::compute(unsigned int threads)
{
	unsigned int neuron_count = neurons.size();
	if (neuron_count == 0)
		return;
	if (threads == 0)
		threads = std::thread::hardware_concurrency();
	unsigned int _threads_count = threads <= neuron_count ? threads : neuron_count;
	std::vector<std::thread> _threads; _threads.reserve(_threads_count);
	
	std::function<void(std::vector<Neuron>*)> _process = [=](std::vector<Neuron>* c) {
		for (Neuron& n : *c)
			n.compute();
	};

	std::vector<std::vector<Neuron>> divided; divided.reserve(threads);

	{
		unsigned int size = neuron_count / threads;
		auto last_begin = neurons.begin();
		for (unsigned int p = 0; p < _threads_count-1; p++) {
			std::vector<Neuron> v(last_begin, last_begin + size);
			last_begin = last_begin + size;
			divided.push_back(v);
		}
		std::vector<Neuron> v(last_begin, neurons.end());
		divided.push_back(v);
	}

	for (unsigned int thread_index = 0; thread_index < threads; thread_index++) {
		_threads.push_back(std::thread(_process, &divided.at(thread_index)));
	}

	for (std::thread& t : _threads) {
		t.join();
	}

}
