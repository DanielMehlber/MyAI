#include "CNN.h"



myai::cnn::Neuron::Neuron()
{
}

myai::cnn::Neuron::Neuron(Layer* owner) : owner{owner}
{
	if (owner->previous)
		weights.resize(owner->previous->count);
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
	myai_dlog("Generation: CNN generation started.");
	Layer* prev = nullptr;
	for (int i : t) {
		Layer* l = new Layer(i, prev);
		layers.push_back(l);
		prev = l;
		myai_dlog("Generation: Layer "<<layers.size()-1<<" (neuron count: "<<i<<") generated.");
	}
	myai_dlog("Generation: finished.");

}

myai::cnn::CNN::~CNN()
{
	myai_dlog("Destruction: started.");
	for (Layer* l : layers)
		l->~Layer();
	myai_dlog("Destruction: completed.");
}

void myai::cnn::CNN::compute()
{
	myai_dlog("Computation: started.");
	unsigned int layer_count = layers.size();
	for (unsigned int i = 0; i < layer_count; i++) {
		layers[i]->compute();
	}
	myai_dlog("Destruction: completed.");
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
	myai_dlog("Saving: started.");
	bio::lw::StaticBinaryBuffer buffer;

	try {
		buffer.open(dest, bio::lw::IO::WRITE);
	}
	catch (std::exception& e) {
		myai_dlog("Error while opening file: " << e.what());
		throw;
	}

	/*Network Information Segment*/
	buffer << VERSION;
	myai_dlog("Saving: Version " << VERSION);
	unsigned int layer_count = layers.size();
	buffer << layer_count;

	/*Layer Structure Segment*/
	Layer* previous = nullptr;
	myai_dlog("Saving: "<<layer_count<<" Layers to be saved.");
	//layer by layer
	for (unsigned int layer_index = 0; layer_index < layer_count; layer_index++) {
		Layer* layer = layers[layer_index];
		unsigned int neuron_count = layer->count;
		buffer << neuron_count;
		//neuron by neuron
		for (unsigned int neuron_index = 0; neuron_index < neuron_count; neuron_index++) {
			Neuron& neuron = layer->neurons[neuron_index];
			buffer << neuron.bias;
			//weight by weight
			unsigned int weights_count = previous ? previous->count : 0;
			for (unsigned int weight_index = 0; weight_index < weights_count; weight_index++) {
				buffer << neuron.weights[weight_index];
			}
		}
		myai_dlog("Saving: Layer "<<layer_index<<" finished. ");

		previous = layer;
	}

	buffer.write();
	buffer.close();
	
}

void myai::cnn::CNN::load(const char* src)
{
	clear();
	myai_dlog("Loading: Network cleared.");
	//Value of counts is always undefined int

	bio::lw::StaticBinaryBuffer buffer; buffer.open(src, bio::lw::READ);
	myai_dlog("Loading: Buffer loaded.");
	float version;
	buffer >> version;
	if (version == VERSION) {
		myai_dlog("Loading: Version "<<version<<" confirmed.");
	} else {
		myai_dlog("Loading: Version "<<version<<" not compatible to "<<VERSION);
		throw std::exception("Version error when loading CNN: Wrong version.");
	}
	unsigned int layer_count = 0;
	buffer >> layer_count;
	myai_dlog("Loading: Serializing "<<layer_count<<" Layers...");
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

			for (unsigned int weight_index = 0; weight_index < weights_count && weights_count != 0; weight_index++) {
				buffer >> neuron.weights[weight_index];
			}
		}
		previous = layer;
		myai_dlog("Loading: Layer "<<layer_index<< " finished. ");
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
