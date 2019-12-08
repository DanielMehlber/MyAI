#pragma once
#include "Network.h"
#include <vector>
#include "Utils.h"
#include <thread>
#include <string>
#include "BinarIO/BinarIO/lwIO.h"
#include <limits> //DEBUG REMOVE
using namespace myai::types;

namespace myai {

	namespace cnn{

		class Layer;

		class Neuron{
		public:
			float activation, bias;
			Layer* owner; //Is null, if there's no previous layer
			std::vector<float> weights;
			Neuron();
			Neuron(Layer* owner);
			~Neuron();

			void compute();

			inline void operator=(float f) {
				activation = f;
			}

		};

		class Layer{
		public:
			std::vector<Neuron> neurons;
			unsigned int count;
			Layer* previous;

			Layer();
			Layer(unsigned int size, Layer* previous);
			~Layer();

			inline Neuron& operator[](unsigned int i) {
				return neurons[i];
			};

			inline void operator=(float* f) {
				for (unsigned int i = 0; i < count; i++)
					neurons[i] = f[i];
			}

			void compute();
			void compute(unsigned int thread_count);
		};

		class CNN : public myai::Network{
		public:
			std::vector<Layer*> layers;
			CNN(std::initializer_list<int> t);
			~CNN();

			inline Layer* operator[](unsigned int i) {
				return layers[i];
			};

			Layer& input() {
				return (*layers.front());
			}

			Layer& output() {
				return (*layers.back());
			}

			void compute();
			void compute(unsigned int thread_count);
			void save(const char* dest);
			void load(const char* src);
			void clear();

		};

	};

}