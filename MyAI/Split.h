#pragma once
#include <vector>

template <typename Type>
class Split {
private:
	unsigned int split_count;
	Type** splits;
public:
	const unsigned int size;
	unsigned int split_count;
	Split(unsigned int size, unsigned int split_size) : size{ size }, split_size{split_size} {
		unsigned int full_split_count = size / split_size;
		unsigned int leftovers = size % split_size;
		bool just_full_splits = leftovers == 0;
		unsigned int total_splits = just_full_splits ? full_split_count : full_split_count + 1;
		splits = new Type * [total_splits];
		for (unsigned int i = 0; i < full_split_count; i++) {
			splits[i] = new Type [split_size];
		}
		if (just_full_splits)
			splits[total_splits - 1] = new Type[leftovers];
	}

	~Split() {
		for (unsigned int i = 0; i < split_count + 1; i++) {
			delete[] splits[i];
		}
	}

	inline Type& operator[] (int i) {
		return splits[i / split_size][i % split_size];
	}

};