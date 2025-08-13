#pragma once
#include <functional>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <memory>
#include <atomic>


class TaskSystem
{
private:
	int maxThreads;
	int activeThreads;

	std::vector<std::thread> workers;
	std::atomic_bool running;

	std::mutex mutex;

	std::queue<std::function<void()>> taskQueue;
	std::atomic_int32_t taskCount;


	// public and private functions
public:

	TaskSystem(int toStart = 0) // toStart = worker threads to initialize, leave at 0 for max
	{
		maxThreads = std::thread::hardware_concurrency();
		workers.reserve(maxThreads);
		running = true;
		taskCount = 0;

		if (toStart <= maxThreads && toStart > 0) activeThreads = toStart;
		else activeThreads = maxThreads;

		for (int i = 0; i < activeThreads; i++) workers.emplace_back(&TaskSystem::Worker, this);
	}

	~TaskSystem() // stops all workers threads
	{
		{
			std::unique_lock<std::mutex> lock(mutex);
			running = false;
		}

		for (int i = 0; i < workers.size(); i++) workers[i].join();
	}


	void AddWorker() // adds a single worker
	{
		if (activeThreads == maxThreads) return;
		workers.emplace_back(&TaskSystem::Worker, this);
		activeThreads++;
	}

	uint8_t MaxThreads() const { return maxThreads; } // returns max thread count
	uint8_t ActiveThreads() const { return activeThreads; } // returns active thread count


	template <typename F, typename... Args>
	inline void AddTask(F&& f, Args&&... args) // adds a function to the task queue for threads to execute, to pass a templated function must pass the template values in <> example: func<int, int>
	{
		std::lock_guard<std::mutex> lock(mutex);
		taskQueue.emplace(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		taskCount++;
	}

	inline void WaitForComplete() // wait for all current tasks to be done before continuing
	{
		while (taskCount > 0) std::this_thread::yield();
	}

	inline void WaitForEmpty() // wait for task queue to be empty
	{
		while (!taskQueue.empty()) std::this_thread::yield();
	}


private:

	void Worker()
	{
		std::function<void()> task = nullptr;
		while (running)
		{
			{
				std::lock_guard<std::mutex> lock(mutex);
				if (taskQueue.empty()) continue;
				task = std::move(taskQueue.front());
				taskQueue.pop();
			}
			if (task == nullptr) std::this_thread::yield();
			else
			{
				task();
				taskCount--;
				task = nullptr;
			}

		}
	}
};