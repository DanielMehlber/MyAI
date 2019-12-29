#include "CNN.h"



myai::cnn::Neuron::Neuron(Layer* owner) : owner{ owner }, activation{ 0 }, bias{0}
{
	if (owner->previous)
		weights.resize(owner->previous->neuron_count);
}

myai::cnn::Neuron::~Neuron()
{
	weights.~vector();
}

void myai::cnn::Neuron::compute()
{
	activation = 0;
	for (unsigned int i = 0; i < owner->previous->neuron_count; i++) {
		activation += owner->previous->operator[](i).activation * weights[i];
	}
	activation += bias;
	activation = utils::clamp(activation);
}


myai::cnn::Layer::Layer(unsigned int size, Layer* previous) : neuron_count{ size }, previous{previous}
{
	neurons.reserve(size);
	for (unsigned int i = 0; i < size; i++)
		neurons.emplace_back(Neuron(this));

}

myai::cnn::Layer::~Layer()
{
	neurons.~vector();
}

void myai::cnn::Layer::compute()
{
	if (!previous)
		return;
	for (Neuron& n : neurons)
		n.compute();

}

#define LOG_GEN_STARTED myai_dlog("Generation: CNN generation started.");
#define LOG_GEN_LAYER_FINISHED_INFO myai_dlog("Generation: Layer "<<layers.size()-1<<" (neuron count: "<<i<<") generated.");
#define LOG_GEN_FINISHED myai_dlog("Generation: finished.");

myai::cnn::CNN::CNN(std::initializer_list<int> t)
{
	LOG_GEN_STARTED
	data.layer_count = 0;
	Layer* prev = nullptr;
	for (int i : t) {
		Layer* l = new Layer(i, prev);
		layers.push_back(l);
		prev = l;
		data.layer_count++;
		LOG_GEN_LAYER_FINISHED_INFO
	}
	LOG_GEN_FINISHED

}

myai::cnn::CNN::CNN(unsigned int* layer_data, unsigned int layer_count)
{
	LOG_GEN_STARTED
	data.layer_count = layer_count;
	layers.resize(layer_count);
	Layer* previous = nullptr;
	for (unsigned int i = 0; i < layer_count; i++) {
		previous = new Layer(layer_data[i], previous);
		layers[i] = previous;
		LOG_GEN_LAYER_FINISHED_INFO
	}
	LOG_GEN_FINISHED
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
	if (thread_count < 0)
		return;
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

	bio::lw::StaticBinaryBuffer buffer; 
	try {
		buffer.open(dest, bio::lw::WRITE);
	}
	catch (bio::types::exception & e) {
		e.pack_info(__FUNCTION__, "Cannot open file " + std::string(dest));
		throw myai::types::exception(e);
	}

	buffer << VERSION; myai_dlog("Saving: Version " << VERSION);

	//DATA SECTION
	buffer << data.layer_count; myai_dlog("Saving: " << data.layer_count << " layer will be saved.");
	buffer << data.accuracy;

	Layer* previous = nullptr;
	for (unsigned int layer_index = 0; layer_index < data.layer_count; layer_index++) {
		Layer* current_layer = layers[layer_index];
		myai_ldlog("Saving: Layer " << layer_index << "...");

		unsigned int neuron_count = (unsigned int)current_layer->neuron_count; buffer << neuron_count;
		for (unsigned int neuron_index = 0; neuron_index < neuron_count; neuron_index++) {
			Neuron& current_neuron = current_layer->neurons[neuron_index];
			buffer << current_neuron.bias;

			unsigned int weight_count = previous ? previous->neuron_count : 0;
			for (unsigned int weight_index = 0; weight_index < weight_count; weight_index++) {
				buffer << current_layer->neurons[neuron_index].weights[weight_index];
			}
		}

		dlog(" finished. " << neuron_count << " neurons serialized.");
		previous = current_layer;
	}
	try {
		buffer.finish();
	}
	catch (bio::types::exception & e) {
		e.pack_info(__FUNCTION__, "Cannot finish buffer.");
		throw types::exception(e);
	}
}


void myai::cnn::CNN::load(const char* src)
{
	clear();
	bio::lw::StaticBinaryBuffer buffer; 
	try {
		buffer.open(src, bio::lw::READ); 
		buffer.read();
	}
	catch (bio::types::exception & e) {
		e.pack_info(__FUNCTION__, "Cannot open and read "+ std::string(src));
		throw types::exception(e);
	}
	float version; buffer >> version;
	
	if (version == VERSION) {
		myai_dlog("Loading: Version " << version << " confirmed. Continue.");
	}
	else {
		myai_dlog("Loading: Version " << version << " not matching program version " << VERSION 
			<< ". Abort." );
		throw types::exception(__FUNCTION__, "Fileversions do not match.");
	}


	//DATA SECTION
	buffer >> data.layer_count;
	buffer >> data.accuracy;

	myai_dlog("Loading: " << data.layer_count << " layers will be loaded.");

	Layer* previous = nullptr;
	for (unsigned int layer_index = 0; layer_index < data.layer_count; layer_index++) {
		unsigned int neuron_count; buffer >> neuron_count;
		Layer* current_layer = new Layer(neuron_count, previous);
		myai_ldlog("Loading: Layer " << layer_index << " containing " << neuron_count << " neurons...");
		for (unsigned int neuron_index = 0; neuron_index < neuron_count; neuron_index++) {
			float bias; buffer >> bias;
			current_layer->neurons[neuron_index].bias = bias;

			unsigned int weight_count = previous ? previous->neuron_count : 0;
			for (unsigned int weight_index = 0; weight_index < weight_count; weight_index++) {
				float weight_value; buffer >> weight_value;
				current_layer->neurons[neuron_index].weights[weight_index] = weight_value;
			}
		}

		dlog(" finished.");
		layers.push_back(current_layer);
		previous = current_layer;
	}

	buffer.close();
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
