#pragma once
#include <memory>
#include <thread>
#include <vector>
#include <functional>
#include <mutex>
#include <iostream>
#include "BinarIO/BinarIO/Utils.h"
#define BIG_ARRAY_SPLIT_SIZE 500
#define print(x) std::cout << x;
#define println(x) std::cout << x << std::endl;

#define myai_API __declspec(dllexport)

namespace myai {

	namespace process {
		
		class threadmgr {
		private:
			unsigned int processes_finshed;
		public:
			const unsigned int process_count;
			std::mutex locker;
			std::vector<std::function<void()>> tasks;
			std::thread* head;
			bool wait_for_tasks = true;
			threadmgr() = delete;
			threadmgr(unsigned int process_count);
			~threadmgr();
			void start();
			std::thread* start(unsigned int left);
			void finish();

			inline void add(std::function<void()> func) {
				locker.lock();
				tasks.push_back(func);
				locker.unlock();
			}

			inline void operator+=(std::function<void()> func) {
				add(func);
			}
		};

		class smart_threadmgr : public process::threadmgr{
		public:
			smart_threadmgr();
		};

	}

	namespace types {
		
		class exception {
		public:
			std::string log;

			myai_API exception(const char* location, std::string message = "Exception caught.");
			myai_API exception(bio::types::exception& e);
			myai_API ~exception();
			myai_API void printLog();
			myai_API void pack_info(const char* location, std::string reason = "Exception caught.");
			
		};
		

		template <typename Type>
		class big_array {
		private:
			class Splits { // So it can be copied
			public:
				Type** splits;
				unsigned int split_count, split_size;
				~Splits() {
					for (unsigned int i = 0; i < split_count; i++)
						delete[] splits[i];
				}
			};

			std::shared_ptr<Splits> splits_ptr;
		public:
			unsigned int size;
			big_array(unsigned int size, unsigned int split_size) : size{ size } {
				splits_ptr = std::make_shared<Splits>();
				unsigned int full_split_count = size / split_size;
				unsigned int leftovers = size % split_size;
				bool just_full_splits = leftovers == 0;
				splits_ptr->split_count = just_full_splits ? full_split_count : full_split_count + 1;
				splits_ptr->split_size = split_size;
				splits_ptr->splits = new Type * [splits_ptr->split_count];
				for (unsigned int i = 0; i < full_split_count; i++) {
					splits_ptr->splits[i] = new Type[split_size];
				}
				if (!just_full_splits)
					splits_ptr->splits[splits_ptr->split_count - 1] = new Type[leftovers];
			}


			~big_array() {
				if (!splits_ptr||splits_ptr->splits)
					return;
				for (unsigned int i = 0; i < splits_ptr->split_count; i++)
					delete[] splits_ptr->splits[i];
			}

			big_array() {

			};


			inline Type& operator[] (int i) {
				return splits_ptr->splits[i / splits_ptr->split_size][i % splits_ptr->split_size];
			}

			inline void operator=(big_array<Type> another) {
				this->size = another.size;
				this->splits_ptr = another.splits_ptr;
			}

		};

	}

	namespace time {
		template <typename unit = std::chrono::milliseconds> class clock {
		private:
			std::chrono::steady_clock::time_point last;
		public:
			clock() {
				last = std::chrono::steady_clock::now();
			};

			inline double stop() {
				auto now = std::chrono::steady_clock::now();
				auto duration = std::chrono::duration_cast<unit>(now - last);
				last = now;
				return duration.count();
			};
		};
	}

	

}