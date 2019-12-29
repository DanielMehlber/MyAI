#include "Utils.h"


myai::process::threadmgr::threadmgr(unsigned int process_count) : process_count{process_count}
{
}

myai::process::threadmgr::~threadmgr()
{
	if (head && head->joinable())
		head->join();
}

void myai::process::threadmgr::start()
{
	head = this->start(process_count);
}

std::thread* myai::process::threadmgr::start(unsigned int left)
{
	if (left == 0)
		return nullptr;
	std::thread* next = start(left - 1);
	std::thread* th = new std::thread([=]() {
		while (wait_for_tasks) {
			std::function<void()> fn = NULL;
			locker.lock();
			if (tasks.size() > 0) {
				fn = tasks.back();
				tasks.pop_back();
			}
			locker.unlock();
			if (fn != NULL)
				fn();
		}
		if (next) {
			next->join();
			next->~thread();
		}
	});
	return th;
}

void myai::process::threadmgr::finish()
{
	if (!head)
		return;
	wait_for_tasks = false;
	head->join();
}

myai::process::smart_threadmgr::smart_threadmgr() : threadmgr(std::thread::hardware_concurrency())
{
}

myai::types::exception::exception(const char* location, std::string message)
{
	log = "MyAI Exception thrown! Callstack:\n";
	pack_info(location, message);
}

myai::types::exception::exception(bio::types::exception& e) {
	log = e.log;
	log += "... Switching from bio_API to myai_API here.\n";
}

myai::types::exception::~exception()
{
	log.~basic_string();
}

void myai::types::exception::printLog()
{
	std::cerr << log;
}

void myai::types::exception::pack_info(const char* location, std::string reason)
{
	log += "... in " + std::string(location) + ": " + reason + "\n";
}
