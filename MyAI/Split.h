#pragma once
#include <memory>
#include <thread>
#include <vector>
#include <functional>

#define BIG_ARRAY_SPLIT_SIZE 500

namespace myai {

	namespace types {

		class processmgr;
		class processmgr {
		public:
			class process_slot {
			public:
				processmgr* manager;
				process_slot(processmgr* mgr);
				inline void start() {
					while (!manager->stop_when_finished) {
						if (manager->slots_finished > 0)
							manager->slots_finished--;
						while (manager->functions.size() > 0) {
							std::function<void(void)> func = manager->functions.back(); manager->functions.pop_back();
							func();
						}
						manager->slots_finished++;
					}
				}
			};

			unsigned int thread_count;
			unsigned int slots_finished;
			bool stop_when_finished;
			std::vector<std::function<void()>> functions;

			void finish();
			inline void add(std::function<void(void)> func) {
				functions.push_back(func);
			}
			void process();
			inline void operator+=(std::function<void()> func) {
				add(func);
			}
		};

		template <typename Type>
		class big_array {
		private:

			class Splits {
			public:
				Type** splits;
				unsigned int split_count;
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
				return splits_ptr->splits[i / BIG_ARRAY_SPLIT_SIZE][i % BIG_ARRAY_SPLIT_SIZE];
			}

			inline void operator=(big_array<Type> another) {
				this->size = another.size;
				this->splits_ptr = another.splits_ptr;
			}

		};

	}

}