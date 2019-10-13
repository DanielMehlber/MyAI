#pragma once
#include "Network.h"
#include <vector>
#include "Utils.h"
#include <thread>
#include <atomic>


using namespace myai::types;

namespace myai {

	namespace cnn {

		class Layer;

		class Neuron {
		public:
			float activation, bias;
			Layer* prev; //Is null, if there's no previous layer
			std::vector<float> weights;
			Neuron() = delete;
			Neuron(Layer* owner);
			~Neuron();

			void compute();
		};

		class Layer {
		public:
			std::vector<Neuron> neurons;
			const unsigned int count;

			Layer() = delete;
			Layer(unsigned int size, Layer* prev);
			~Layer();

			inline Neuron& operator[](unsigned int i) {
				return neurons[i];
			};


			void compute();
			void compute(unsigned int thread_count);

		};

		class CNN : myai::Network {
		public:
			std::vector<Layer> layers;

			CNN(std::initializer_list<int> t);

			inline Layer& operator[](unsigned int i) {
				return layers[i];
			};

			void compute();
			void compute(unsigned int thread_count);
			

		};

	};

}