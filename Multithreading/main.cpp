#include <cstdlib>
#include <string>
#include <iostream>
#include <assert.h>
#include <atomic>

#include "DeferredTasksExecutor.h"

const char *task_status_to_string(TaskStatus status)
{
	switch (status) {
	case TaskStatus::not_in_queue: return "not_in_queue";
	case TaskStatus::in_queue: return "in_queue";
	case TaskStatus::processing: return "processing";
	case TaskStatus::cancel: return "cancel";
	case TaskStatus::canceled: return "canceled";
	case TaskStatus::done: return "done";
	default: return "unknown";
	}
}

void trace(const std::string str)
{
	static std::mutex trace_mtx;
	std::lock_guard<std::mutex> lock(trace_mtx);
	std::cout << str;
}

int main2()
{
	std::srand(static_cast<int>(std::chrono::system_clock::now().time_since_epoch().count()));

	DeferredTasksExecutor &executor = DeferredTasksExecutor::get_instance();
	std::cout << "Pool size: " << executor.pool_size() << std::endl;	

	size_t tasks_count = 1000;
	auto test_task = []() {
		std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 100));
	};

	{
		std::cout << std::endl;
		std::cout << "### Generate tasks and wait for all done ###" << std::endl;

		size_t added_tasks = 0;
		std::atomic<size_t> completed_tasks = 0;

		std::cout << "=> Generate " << tasks_count << " tasks" << std::endl;
		for (size_t i = 0; i < tasks_count; i++) {
			added_tasks++;
			executor.add_task([&completed_tasks]() {
				std::cout << ".";
				std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 100));
				completed_tasks++;
			});
		}
		executor.wait_for_all_done();

		std::cout << std::endl << "All tasks done" << std::endl;
		std::cout << "# Task added: " << added_tasks << std::endl;
		std::cout << "# Task completed: " << completed_tasks << std::endl;
	}


	{
		std::cout << std::endl << std::endl;
		std::cout << "### Wait task for done  ###" << std::endl;

		std::cout << "=> Generate " << tasks_count << " tasks" << std::endl;
		for (size_t i = 0; i < tasks_count; i++) {
			executor.add_task(test_task);
		}

		std::cout << "=> Add <task_1>" << std::endl;
		auto task = executor.add_task([]() {
			std::this_thread::sleep_for(std::chrono::seconds(5));
		});

		std::cout << "=> Generate " << tasks_count << " tasks" << std::endl;
		for (size_t i = 0; i < tasks_count; i++) {
			executor.add_task(test_task);
		}

		std::cout << "Wait for <task_1>" << std::endl;
		task->wait_for_done();
		std::cout << "<task_1> status: " << task_status_to_string(task->status()) << std::endl;

		std::cout << "Wait for all done" << std::endl;
		executor.wait_for_all_done();
		std::cout << "All done" << std::endl;
	}


	{
		std::cout << std::endl << std::endl;
		std::cout << "### Test task status ###" << std::endl;

		std::cout << "=> Generate " << tasks_count << " tasks" << std::endl;
		for (size_t i = 0; i < tasks_count; i++) {
			executor.add_task(test_task);
		}

		std::cout << "=> Add <task_1>" << std::endl;
		auto task = executor.add_task([]() {
			std::this_thread::sleep_for(std::chrono::seconds(5));
		});

		std::cout << "<task_1> status: " << task_status_to_string(task->status()) << std::endl;
		while (task->status() != TaskStatus::done) {
			task->wait_for_status_changed();
			std::cout << "<task_1> status: " << task_status_to_string(task->status()) << std::endl;
		}
		executor.wait_for_all_done();
	}


	{
		std::cout << std::endl << std::endl;
		std::cout << "### Test tasks with precondition" << std::endl;

		auto task_1 = std::make_shared<Task>([]() {
			std::cout << "<task_1> processing" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(2));
			std::cout << "<task_1> done" << std::endl;
		});

		std::cout << "=> Add <task_2> with precondition: task_1.is_done()" << std::endl;
		auto task_2 = executor.add_task([]() {
			std::cout << "<task_2> processing" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(2));
			std::cout << "<task_2> done" << std::endl;
		}, 0,
			[task_1]() {return task_1->is_done();}
		);

		std::cout << "=> Add <task_3> with precondition: task_2.is_done()" << std::endl;
		auto task_3 = executor.add_task([]() {
			std::cout << "<task_3> processing" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(2));
			std::cout << "<task_3> done" << std::endl;
		}, 0,
			[task_2]() {return task_2->is_done(); }
		);

		std::cout << "=> Generate " << tasks_count << " tasks" << std::endl;
		for (size_t i = 0; i < tasks_count; i++) {
			executor.add_task(test_task);
		}
		
		std::cout << "=> Add <task_1>" << std::endl;
		executor.add_task(task_1);

		std::cout << "Wait for all done" << std::endl;
		executor.wait_for_all_done();
		std::cout << "All done" << std::endl;	
	}


	{	
		std::cout << std::endl << std::endl;
		std::cout << "### Test tasks with priority" << std::endl;

		std::cout << "=> Generate " << tasks_count << " tasks" << std::endl;
		for (size_t i = 0; i < tasks_count; i++) {
			executor.add_task([]() {
				trace(".");
				std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 100));
			});
		}

		trace("\n=> Add task with higher priority\n");
		auto priority_task = executor.add_task([]() {
			trace("\nHigh priority task done\n");
		}, 10);

		executor.wait_for_all_done();
		std::cout << std::endl << "All done" << std::endl;
	}
	

	{
		std::cout << std::endl << std::endl;
		std::cout << "### Test cancel task" << std::endl;

		std::cout << "=> Generate " << tasks_count << " tasks" << std::endl;
		for (size_t i = 0; i < tasks_count; i++) {
			executor.add_task(test_task);
		}

		std::cout << "=> Add <task_1> task for canceling" << std::endl;
		auto task = executor.add_task([]() {
			std::cout << "Never be done" << std::endl;
		});
		std::cout << "<task_1> status: " << task_status_to_string(task->status()) << std::endl;

		std::cout << "<= Cancel <task_1>" << std::endl;
		executor.cancel_task(task);

		std::cout << "<task_1> status: " << task_status_to_string(task->status()) << std::endl;
		while (task->status() != TaskStatus::canceled) {
			task->wait_for_status_changed();
			std::cout << "<task_1> status: " << task_status_to_string(task->status()) << std::endl;
		}

		std::cout << "Wait for all done" << std::endl;
		executor.wait_for_all_done();
		std::cout << "All done" << std::endl;
	}

    return 0;
}

int main() 
{
	size_t pass_counter = 0;
	while (true) {
		std::cout << std::endl;
		std::cout << "### PASS " << pass_counter++ << std::endl;
		main2();
	}
	return 0;
}
