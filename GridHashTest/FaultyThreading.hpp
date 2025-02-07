#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>


class TaskQueue 
{
public:

	// adds a task to the task queue
	void AddTask(std::function<void()>&& task)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		tasks.push(std::move(task));
		condition.notify_one();

		taskCount++;
	}

	// grabs a task and pops it off the task queue.
	bool GetTask(std::function<void()>& task)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		if (tasks.empty()) return false;

		task = std::move(tasks.front());
		tasks.pop();

		return true;
	}

	// checks if the tasks queue is empty;
	bool Empty()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		return tasks.empty();
	}

	// gets the condition
	std::condition_variable& GetCondition()
	{
		return condition;
	}

	// gets mutex
	std::mutex& GetMutex()
	{
		return _mutex;
	}

	// lets the thread work on other things
	void WaitUntilDone()
	{
		while (taskCount > 0)
		{
			std::this_thread::yield();
		}
	}

	// reduces the task count
	void CompleteTask()
	{
		taskCount--;
	}

private:
	std::queue<std::function<void()>> tasks;
	std::mutex _mutex;
	std::condition_variable condition;
	std::atomic<int> taskCount = 0;
};



class TaskSystem 
{
public:

	TaskSystem(size_t threadCount)
	{
		for (size_t i = 0; i < threadCount; i++)
		{
			workers.emplace_back([this]() {while (working) 
				{
					std::function<void()> task; 
					{
						std::unique_lock<std::mutex> lock(taskQueue.GetMutex());

						taskQueue.GetCondition().wait(lock, [this] { return !taskQueue.Empty() || !working; });
						if (!working && taskQueue.Empty()) return;
					}
					if (taskQueue.GetTask(task))
					{
						task();
					}
				}
			});
		}
	}

	void SubmitTask(std::function<void()>&& task)
	{
		taskQueue.AddTask(std::move(task));
	}

	void Shutdown()
	{
		working = false;
		taskQueue.GetCondition().notify_all();
		for (std::thread& worker : workers)
		{
			if (worker.joinable())
			{
				worker.join();
			}
		}
	}

	~TaskSystem()
	{
		Shutdown();
	}

private:
	std::vector<std::thread> workers;
	TaskQueue taskQueue;
	std::atomic<bool> working = true;
};