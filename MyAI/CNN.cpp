#include "CNN.h"



myai::cnn::Neuron::Neuron()
{
}

myai::cnn::Neuron::Neuron(Layer* owner) : owner{owner}
{
	if(owner)
		weights.resize(owner->count);
}

myai::cnn::Neuron::~Neuron()
{
	weights.~vector();
}

void myai::cnn::Neuron::compute()
{
	activation = 0;
	for (unsigned int i = 0; i < owner->count; i++) {
		activation += owner->previous->operator[](i).activation * weights[i];;
	}
	activation += bias;
	activation = utils::clamp(activation);
}


myai::cnn::Layer::Layer()
{
}

myai::cnn::Layer::Layer(unsigned int size, Layer* previous) : count{ size }, previous{previous}
{
	neurons.reserve(size);
	for (unsigned int i = 0; i < size; i++)
		neurons.push_back(Neuron(this));

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

void myai::cnn::CNN::save(const char* dest)
{
	//precalc size
	unsigned int size = sizeof(unsigned int); //Layer count
	Layer* prev = nullptr;
	for (Layer* l : layers) {
		size += sizeof(unsigned int); //Neuron count
		size += l->count * (sizeof(float)); //Biases
		if (prev) {
			size += prev->count * sizeof(float) * l->count; //weights
		}
		prev = l;
	}

	bio::lw::DynamicBinaryBuffer buffer(size);
	
	unsigned int layer_count = layers.size();
	buffer << layer_count;
	Layer* previous = nullptr;
	//layer by layer
	for (unsigned int layer_index = 0; layer_index < layer_count; layer_index++) {
		//std::cout << "Layer " << layer_index << std::endl;
		Layer* layer = layers[layer_index];
		unsigned int neuron_count = layer->count;
		buffer << neuron_count;
		//neuron by neuron
		for (unsigned int neuron_index = 0; neuron_index < neuron_count; neuron_index++) {
			//std::cout << "Neuron " << neuron_index << std::endl;
			Neuron& neuron = layer->neurons[neuron_index];
			buffer << neuron.bias;
			//weight by weight
			unsigned int weights_count = 0;
			if (previous)
				weights_count = previous->count;
			for (unsigned int weight_index = 0; weight_index < weights_count; weight_index++) {
				//std::cout << "Weight " << weight_index << std::endl;
				buffer << neuron.weights[weight_index];
			}
		}
	}

	buffer.write(dest);
	
}

void myai::cnn::CNN::load(const char* src)
{
	clear();

	//Value of counts is always undefined int

	bio::lw::DynamicBinaryBuffer buffer; buffer.read(src);
	unsigned int layer_count = 0;
	buffer >> layer_count;
	Layer* previous = nullptr;
	//layer by layer
	for (unsigned int layer_index = 0; layer_index < layer_count; layer_index++) {
		unsigned int neuron_count = 0;
		buffer >> neuron_count;
		Layer* layer = new Layer(neuron_count, previous);
		for (unsigned int neuron_index = 0; neuron_index < neuron_count; neuron_index++) {
			Neuron neuron(layer);
			buffer >> neuron.bias;
			unsigned int weights_count = previous ? previous->count : 0;
			neuron.weights.resize(weights_count);
			//weight by weight
			for (unsigned int weight_index = 0; weight_index < weights_count; weight_index++) {
				buffer >> neuron.weights[weight_index];
			}
		}
		previous = layer;
	}
}

void myai::cnn::CNN::clear()
{
	for (Layer* l : layers) {
		l->~Layer();
	}

	layers.clear();
}

void _process(std::vector<myai::cnn::Neuron>* neurons, unsigned int from, unsigned int to) {
	for (unsigned int i = from; i < to; i++) {
		neurons->operator[](i).compute();
	}
}

void myai::cnn::Layer::compute(unsigned int threads) //Performance critical section
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
