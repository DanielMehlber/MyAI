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
	unsigned int size = prev->count;
	activation = 0;
	for (unsigned int i = 0; i < size; i++) {
		float weight = weights[i];
		activation += prev->operator[](i).activation * weight;
	}
	activation += bias;
	activation = utils::clamp(activation);
}


myai::cnn::Layer::Layer(unsigned int size, Layer* prev) : count{ size }, previous{prev}
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
	if (previous == nullptr)
		return;
	for (Neuron& n : neurons)
		n.compute();
}

myai::cnn::CNN::CNN(std::initializer_list<int> t)
{
	Layer* prev = nullptr;
	for (int i : t) {
		Layer* l = new Layer(i, prev);
		layers.push_back(l);
		prev = l;
	}
}

myai::cnn::CNN::~CNN()
{
	for (Layer* l : layers)
		l->~Layer();
}

void myai::cnn::CNN::compute()
{
	unsigned int layer_count = layers.size();
	for (unsigned int i = 0; i < layer_count; i++) {
		layers[i]->compute();
	}
}

void myai::cnn::CNN::compute(unsigned int thread_count)
{
	if (thread_count == 1) {
		compute();
		return;
	}
	for (Layer* l : layers) {
		l->compute(thread_count);
	}
}

void _process(std::vector<myai::cnn::Neuron>* neurons, unsigned int from, unsigned int to) {
	for (unsigned int i = from; i < to; i++) {
		neurons->operator[](i).compute();
	}
}

void myai::cnn::Layer::compute(unsigned int threads)
{
	if (!previous)
		return;
	
	unsigned int neuron_count = neurons.size();
	if (neuron_count == 0)
		return;
	if (threads == 0)
		threads = std::thread::hardware_concurrency();
	unsigned int _threads_count = threads <= neuron_count ? threads : neuron_count;
	std::vector<std::thread> _threads; _threads.reserve(_threads_count);
	

	unsigned int per_thread = neuron_count / _threads_count;

	unsigned int last_start_index = 0;
	unsigned int last_end_index = per_thread;
	for (unsigned int thread_index = 0; thread_index < _threads_count-1; thread_index++) {
		last_end_index = (thread_index + 1) * per_thread;
		_threads.push_back(std::thread(_process, &neurons, last_start_index, last_end_index));
		last_start_index = last_end_index + 1;
	}
	if (neuron_count > last_end_index + 1)
		_threads.push_back(std::thread(_process, &neurons, last_start_index, neuron_count-1));

	for (std::thread& t : _threads) {
		t.join();
	}

}
