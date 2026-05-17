#pragma once
#include <functional>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <atomic>
#include <condition_variable>


class TaskSystem
{
private:
	size_t maxThreads;
    size_t  activeThreads;

	std::vector<std::thread> workers;
	std::atomic_bool running;

	std::mutex mutex;

	std::queue<std::function<void()>> taskQueue;
	std::atomic_int32_t taskCount;

    std::condition_variable cv;


public:

	TaskSystem(size_t toStart = 0) 
	{
		maxThreads = std::thread::hardware_concurrency();
        if (maxThreads == 0) maxThreads = 1;

		workers.reserve(maxThreads);
		running = true;
		taskCount = 0;

		if (toStart <= maxThreads && toStart > 0) activeThreads = toStart;
		else activeThreads = maxThreads;

		for (size_t i = 0; i < activeThreads; i++) workers.emplace_back(&TaskSystem::Worker, this);
	}

	~TaskSystem() // stops all worker threads
	{
		{
			std::unique_lock<std::mutex> lock(mutex);
			running = false;
		}

        cv.notify_all();
		for (size_t i = 0; i < workers.size(); i++) workers[i].join();
	}


	inline size_t MaxThreads() const { return maxThreads; } 
	inline size_t ActiveThreads() const { return activeThreads; } 
	

	template <typename F, typename... Args>
	inline void AddTask(F&& f, Args&&... args) {
        {
		    std::lock_guard<std::mutex> lock(mutex);
		    taskQueue.emplace( [func = std::forward<F>(f), ...args = std::forward<Args>(args)]() mutable { func(std::move(args)...); } );
		    taskCount++;
        }

        cv.notify_one();
	}


	inline void WaitForComplete() { while (taskCount > 0) std::this_thread::yield(); }
	inline void WaitForEmpty() { while (!taskQueue.empty()) std::this_thread::yield(); }
	 

private:

	void Worker() {	
		std::function<void()> task = nullptr;
		while (running) 
        {
			{
				std::unique_lock<std::mutex> lock(mutex);
                cv.wait(lock, [this] { return !running || !taskQueue.empty(); });

			    if (!running && taskQueue.empty()) return;

				task = std::move(taskQueue.front());
				taskQueue.pop();
			}

			task();
			taskCount--;
			task = nullptr;
		}
	}
};
