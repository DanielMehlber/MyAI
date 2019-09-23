#include "Split.h"
#include "Utils.h"

myai::types::processmgr::processmgr(unsigned int thread_count) : thread_count{thread_count}
{
}

void myai::types::processmgr::finish()
{
	stop_when_finished = true;
	while (slots_finished != thread_count);
}

void myai::types::processmgr::process()
{
	stop_when_finished = false;
	std::vector<_process_slot> slots;
	for (unsigned int i = 0; i < thread_count; i++) {
		_process_slot slot(this);
		slots.push_back(slot);
		std::thread t(&_process_slot::start, slot);
	}
}

myai::types::_process_slot::_process_slot(processmgr* mgr) : manager{mgr}
{
}
