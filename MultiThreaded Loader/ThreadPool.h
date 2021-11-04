#pragma once
#include <thread>
#include <vector>
#include "CTask.h"
#include "WorkQueue.h"
#include <list>
#include <queue>
#include <mutex>
#include <memory>
std::mutex mut;
class ThreadPool
{
public:
	bool workrun = true;

	~ThreadPool() {};
	void Initialize(std::wstring URL,boolean task) {
		//pushing data into work que
		//std::cout << "pushing data to work que" << i << "\n";
		
		m_pWorkQueue.push(CTask(URL, task));
		
	};
	void Submit() {// threads fetch and execute tasks
		
		while (workrun == true)
		{
			
			mut.lock();
			int size = m_pWorkQueue.size();
				if (size > 0)
				{
					CTask Task;
					m_pWorkQueue.nonblocking_pop(Task);
					Task.task();
					
					//MessageBox(HWD, L"CTask", L"CTASK", MB_ICONASTERISK);
					
				}
				
				
			mut.unlock();
			//std::this_thread::sleep_for(std::chrono::milliseconds(50));
						
		}

		
		return;
	};

	void DoWork() 
	{
		
	};
	void Start()
	{
		// initialize and stop
		m_iNumberOfThreads = 16;
		std::cout << "start\n";
		

		return;
	};
	void Stop() 
	{
		workrun = false;
		return;
	};

private:
	
protected:
	static ThreadPool * s_pThreadPool;
private:
	//The Threadpool has a workqueue
	WorkQueue<CTask > m_pWorkQueue;
	//The ThreadPool has a number of worker threads
	std::vector<std::thread> m_workerThreads;
	//A variable to hold the number of threads we want in the pool
	int m_iNumberOfThreads = 16;
	HWND HWD=0;
};