#pragma once
#include "Network.h"
#include <vector>
#include "Utils.h"
#include <thread>
#include <string>
#include "BinarIO/BinarIO/lwIO.h"
#include <limits> //DEBUG REMOVE

#define myai_API __declspec(dllexport)

#define AUTO_THREAD_COUNT std::thread::hardware_concurrency()

using namespace myai::types;

namespace myai {

	namespace cnn{

		class Layer;

		class Neuron{
		public:
			float activation /*Activation of neuron*/, bias /*Bias of neuron*/;
			Layer* owner; /*Layer of neuron*/
			std::vector<float> weights; /*Weights of neurons of previous layer*/

			myai_API Neuron() = delete;
			myai_API Neuron(Layer* owner);
			myai_API ~Neuron();

			/*computes activation of neuron*/
			myai_API void compute();

			/*setter of activation value*/
			myai_API inline void operator=(float f) {
				activation = f;
			}
			/*Converter: returns reference to activation*/
			myai_API inline operator float () {
				return activation;
			}

			/*returns reference to weight value*/
			myai_API inline float& operator[](unsigned int index) {
				return weights[index];
			}
		};

		class Layer{
		public:
			std::vector<Neuron> neurons; /*neurons of layer*/
			unsigned int neuron_count; /*count of neurons*/
			Layer* previous; /*previous layer | Equals nullptr if it's the first one*/

			myai_API Layer() = delete;
			myai_API Layer(unsigned int size, Layer* previous);
			myai_API ~Layer();

			/*returns neuron reference*/
			myai_API inline Neuron& operator[](unsigned int i) {
				return neurons[i];
			};

			/*computes neuron activations without threading (single-thread)*/
			myai_API void compute();
			/*computes neuron activations with threading. Values:
			- auto-thread-count = 0 
			- single-thread = 1
			- specific-count > 1*/
			myai_API void compute(unsigned int thread_count);
		};

		class CNN : public myai::Network{
		public:
			std::vector<Layer*> layers; /*layers of network*/

			struct {
				float accuracy; /*last messured accuracy*/
				unsigned int layer_count; /*amount of layers contained by this cnn*/
			} data /*basic data of cnn*/;

			/*Constructs CNN using list of ints*/
			myai_API CNN(std::initializer_list<int> t);
			myai_API CNN(unsigned int* layers, unsigned int layer_count);
			myai_API ~CNN();

			/*returns layer pointer*/
			myai_API inline Layer* operator[](unsigned int i) {
				return layers[i];
			};

			/*returns reference to input layer*/
			myai_API Layer& input() {
				return (*layers.front());
			}

			/*returns reference to output layer*/
			myai_API Layer& output() {
				return (*layers.back());
			}

			/*computes neuron activations using input from the input layer. No threading (single-thread)*/
			myai_API void compute();
			/*computes neuron activations using input from the input layer. Makes use of threading.
			Possible Values:
			- auto-thread-count = 0 (detects ideal amount of threats suited for machine) << RECOMMENDED
			- single-thread = 1
			- specific-value > 1
			*/
			myai_API void compute(unsigned int thread_count);
			/*Save current cnn to destination*/
			myai_API void save(const char* dest);
			/*Load cnn from source*/
			myai_API void load(const char* src);
			/*Resets cnn to default. No layers, no neurons.*/
			myai_API void clear();

		};

		

	};

}