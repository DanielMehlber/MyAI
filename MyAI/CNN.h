#pragma once
#include "Network.h"
#include <vector>
#include "Utils.h"
#include <thread>
#include <string>
#include "BinarIO/BinarIO/lwIO.h"
#include <limits> //DEBUG REMOVE

#define myai_API __declspec(dllexport)

using namespace myai::types;

namespace myai {

	namespace cnn{

		class Layer;

		class Neuron{
		public:
			float activation, bias;
			Layer* owner; //Is null, if there's no previous layer
			std::vector<float> weights;
			myai_API Neuron();
			myai_API Neuron(Layer* owner);
			myai_API ~Neuron();

			myai_API void compute();

			myai_API inline void operator=(float f) {
				activation = f;
			}

		};

		class Layer{
		public:
			std::vector<Neuron> neurons;
			unsigned int count;
			Layer* previous;

			myai_API Layer();
			myai_API Layer(unsigned int size, Layer* previous);
			myai_API ~Layer();

			myai_API inline Neuron& operator[](unsigned int i) {
				return neurons[i];
			};

			myai_API inline void operator=(float* f) {
				for (unsigned int i = 0; i < count; i++)
					neurons[i] = f[i];
			}

			myai_API void compute();
			myai_API void compute(unsigned int thread_count);
		};

		class CNN : public myai::Network{
		public:
			std::vector<Layer*> layers;
			myai_API CNN(std::initializer_list<int> t);
			myai_API CNN(unsigned int* layers, unsigned int layer_count);
			myai_API ~CNN();

			myai_API inline Layer* operator[](unsigned int i) {
				return layers[i];
			};

			myai_API Layer& input() {
				return (*layers.front());
			}

			myai_API Layer& output() {
				return (*layers.back());
			}

			myai_API void compute();
			myai_API void compute(unsigned int thread_count);
			myai_API void save(const char* dest);
			myai_API void load(const char* src);
			myai_API void clear();

		};

	};

}