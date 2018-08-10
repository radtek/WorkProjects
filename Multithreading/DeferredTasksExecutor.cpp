#include "stdafx.h"

#include <string>
#include <atomic>

#include "DeferredTasksExecutor.h"
#include "trace.h"


DeferredTasksExecutor::DeferredTasksExecutor() : terminate(false)
{
	size_t threads_count = std::thread::hardware_concurrency();
	if (threads_count == 0) {
		threads_count = 2;
	}

	pool.resize(threads_count);
	size_t worker_id = 0;
	try {
		for (auto &thr : pool) {
			thr = std::thread(&DeferredTasksExecutor::worker_func, this, worker_id++);
		}
	} catch (...) {
		terminate_and_join_all_threads();
		throw;
	}
}

DeferredTasksExecutor::~DeferredTasksExecutor()
{
	terminate_and_join_all_threads();
}

void DeferredTasksExecutor::add_task(Task task)
{
	tasks.push(task);
}

void DeferredTasksExecutor::wait_for_complete_all_tasks(void)
{
	while (!tasks.empty()) {
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
}

void DeferredTasksExecutor::worker_func(size_t worker_id)
{
	std::string id_str = std::to_string(worker_id);
	trace("Worker started");

	while (!terminate) {
		Task task;
		if (tasks.try_pop(task)) {
			trace(id_str + " => Execute task");
			task();
		} else {
			std::this_thread::sleep_for(std::chrono::microseconds(100));
		}
	}

	trace("Worker stoped");
}

void DeferredTasksExecutor::terminate_and_join_all_threads(void)
{
	terminate = true;
	for (auto &thr : pool) {
		if (thr.joinable()) {
			thr.join();
		}
	}
}
