#include "Split.h"

void myai::types::processmgr::finish()
{
	stop_when_finished = true;
	while (slots_finished != thread_count);
}

void myai::types::processmgr::process()
{
	stop_when_finished = false;
	std::vector<process_slot> slots;
	for (unsigned int i = 0; i < thread_count; i++) {
		process_slot slot(this);
		slots.push_back(slot);
		std::thread t(&process_slot::start, slot);
	}
}

myai::types::processmgr::process_slot::process_slot(processmgr* mgr) : manager{mgr}
{
}
