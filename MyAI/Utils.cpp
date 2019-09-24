#include "Utils.h"

myai::types::processmgr::processmgr(unsigned int max_process_count)
{
	settings.process_count = max_process_count;
}


void myai::types::processmgr::finish()
{
	settings.stop_when_finished = true;
	while (settings.process_slots_finished != settings.process_count);
}

void myai::types::processmgr::process()
{
	settings.stop_when_finished = false;
	std::vector<_process_slot> slots;
	for (unsigned int i = 0; i < settings.process_count; i++) {
		_process_slot slot = _process_slot(&settings);
		slots.push_back(slot);
		std::thread t(&_process_slot::start, slot);
	}
}

myai::types::processmgr::_process_slot::_process_slot(_mgr_settings* mgr) : settings{mgr}
{
}
