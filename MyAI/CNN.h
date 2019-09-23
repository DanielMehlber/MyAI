#pragma once
#include "Network.h"
#include <vector>
#include "Split.h"
#include <thread>


using namespace myai::types;

namespace myai {

	namespace cnn {

		class Layer;
		

		class Neuron {
		public:
			float activation, bias;
			Layer* prev;
			big_array<float> weights;
			Neuron();
			Neuron(Layer* prev);
			~Neuron();

			void compute();
		};

		class Layer {
		public:
			big_array<Neuron> neurons;

			Layer();
			Layer(unsigned int size);
			~Layer();

			inline Neuron& operator[](unsigned int i) {
				return neurons[i];
			};

			inline unsigned int count() {
				return neurons.size;
			};

			void compute();

		};

		class CNN : myai::Network {
		public:
			std::vector<Layer*> layers;

			CNN(std::initializer_list<Layer*> t);

			inline Layer* operator[](unsigned int i) {
				return layers[i];
			};

			inline void compute() {
				unsigned int layer_count = layers.size();
				for (unsigned int i = 0; i < layer_count; i++) {
					layers[i]->compute();
				}
			}

		};

	};

}