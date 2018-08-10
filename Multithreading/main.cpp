#include "stdafx.h"

#include <cstdlib>
#include <assert.h>
#include <atomic>

#include "DeferredTasksExecutor.h"
#include "TSQueue.h"
#include "trace.h"

void task_func(void)
{
	auto duration = std::chrono::milliseconds(std::rand() % 1000);
	std::this_thread::sleep_for(duration);
}

int main()
{
	std::srand(static_cast<int>(std::chrono::system_clock::now().time_since_epoch().count()));

	size_t added_tasks = 0;
	std::atomic<size_t> executed_tasks = 0;

	size_t tasks_count = 3000;
	{
		try {
			DeferredTasksExecutor executor;
			for (size_t i = 0; i < tasks_count; i++) {
				trace("Add task");
				added_tasks++;
				executor.add_task([&executed_tasks]() {
					std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 100));
					executed_tasks++;
				});
			}
			executor.wait_for_complete_all_tasks();
		} catch (...) {
			int x = 0;
		}
	}
	assert(added_tasks == executed_tasks);

	std::system("pause");
    return 0;
}

