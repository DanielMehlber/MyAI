#pragma once
#include "Network.h"
#include <vector>
#include "Split.h"

namespace cnn {

	class Neuron {
	public:
		float activation, bias;
		Layer* prev;
		Split<float> weights;
		void calc();
	};

	class Layer {
	public:
		Split<Neuron> neurons;

		inline Neuron& operator[](unsigned int i) {
			return neurons[i];
		};

		inline unsigned int count(){
			return neurons.size;
		};

		inline void calc() {
			unsigned int ncount = count();
			for (unsigned int i = 0; i < ncount; i++)
				neurons[i].calc();
		};
	};

	class CNN : Network {
	public:
		std::vector<Layer&> layers;
		Network(std::initializer_list<Layer*> t);
		inline Layer& operator[](unsigned int i) {
			return layers[i];
		};
		
	};

};