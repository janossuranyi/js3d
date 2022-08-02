
#include <thread>
#include <mutex>
#include "thread.h"
#include "logger.h"

namespace js3d {
	
	Thread::Thread(const char* name)
	{
		_name = name;
		_ret = 0;
		_workerWorkDone = false;
		_workerWorkTodo = false;

		_terminate = 0;

		_thread = std::thread(&Thread::threadProc, this);
	}

	Thread::~Thread() noexcept
	{
		std::unique_lock<std::mutex> lock(_mtx);

		_terminate = true;
		_workerWorkTodo = true;
		_workerWorkTodo_cv.notify_one();
		lock.unlock();

		_thread.join();
	}

	int Thread::start_worker()
	{

		int latch = _ret;
		
		std::unique_lock<std::mutex> lock(_mtx);
		_workerWorkDone_cv.wait(lock, [&] {return _workerWorkDone; });
		_workerWorkTodo = true;
		_workerWorkTodo_cv.notify_one();

		return _ret;
	}

	void Thread::wait_for_done()
	{
		std::unique_lock<std::mutex> lock(_mtx);
		_workerWorkDone_cv.wait(lock, [&] {return _workerWorkDone; });
	}

	void Thread::run()
	{
		_ret = 1;
	}

	void Thread::threadProc()
	{
		while (!_terminate)
		{
			std::unique_lock<std::mutex> lock(_mtx);
			_workerWorkDone = true;
			_workerWorkDone_cv.notify_one();
			_workerWorkTodo_cv.wait(lock, [&] {return _workerWorkTodo; });

			_workerWorkTodo = false;
			_workerWorkDone = false;

			if (_terminate)
			{
				break;
			}

			lock.unlock();

			this->run();
		}
		info("worker done!");
	}

}
