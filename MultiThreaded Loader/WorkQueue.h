#pragma once
#include <thread>
#include <memory>
#include <mutex>
#include <queue>
#include <utility>
#include <vector>
#include "CTask.h"
template<typename T>

class WorkQueue
{
public:
	
	//Insert an item at the back of the queue and signal any thread that might be waiting 
	//for the queue to be populated
	void push(const T& item)
	{
		
		std::lock_guard<std::mutex> _lock(m_WorkQMutex);
		workQ.push(std::move(item));
		m_WorkQCondition.notify_one();

	};
	//Attempt to get a workitem from the queue
	//If the queue is empty just return false; 
	bool nonblocking_pop(T& _workItem)
	{

		std::lock_guard<std::mutex> _lock(m_WorkQMutex);
		//If the queue is empty return false
		if (workQ.empty())
		{
			return false;
		}
		std::cout << "pushing data to thread" << "\n";
		_workItem = std::move(workQ.front());
		workQ.pop();
		return true;
	}
	//Attempt to get a workitem from the queue
	//If the Q is empty just return false; 
	void blocking_pop(T& _workItem)
	{
				std::unique_lock<std::mutex> _lock(m_WorkQMutex);
		//If the queue is empty block the thread from running until a work item 
		//becomes available
		m_WorkQCondition.wait(_lock, [this] {return !workQ.empty(); });
		_workItem = std::move(workQ.front());
		workQ.pop();
			}
	//Checking if the queue is empty or not
	int empty() const
	{
		std::lock_guard<std::mutex> _lock(m_WorkQMutex);
		return workQ.empty();
	}

	int size() const
	{
		std::lock_guard<std::mutex> _lock(m_WorkQMutex);
		return workQ.size();
	}

private:
	std::queue<T> workQ;
	mutable std::mutex m_WorkQMutex;
	std::condition_variable m_WorkQCondition;
};