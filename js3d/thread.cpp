
#include <thread>
#include <mutex>
#include "thread.h"
#include "logger.h"

namespace js3d {
	
	Worker::Worker(const char* name)
	{
		_name = name;
		_ret = 0;
		_workerWorkDone = false;
		_workerWorkTodo = false;

		_terminate = 0;

		_thread = std::thread(&Worker::threadProc, this);
	}

	Worker::~Worker() noexcept
	{
		std::unique_lock<std::mutex> lock(_mtx);

		_terminate = true;
		_workerWorkTodo = true;

		lock.unlock();
		_workerWorkTodoCond.notify_one();

		_thread.join();
	}

	int Worker::start_worker()
	{

		int latch = _ret;

		{
			std::unique_lock<std::mutex> lock(_mtx);

			_workerWorkDoneCond.wait(lock, [&] {return _workerWorkDone; });
			_workerWorkTodo = true;
		}

		_workerWorkTodoCond.notify_one();

		return _ret;
	}

	void Worker::wait_for_done()
	{
		std::unique_lock<std::mutex> lock(_mtx);
		_workerWorkDoneCond.wait(lock, [&] {return _workerWorkDone; });
	}

	void Worker::run()
	{
		_ret = 1;
	}

	void Worker::threadProc()
	{
		while (!_terminate)
		{
			std::unique_lock<std::mutex> lock(_mtx);

			_workerWorkDone = true;
			_workerWorkDoneCond.notify_one();

			_workerWorkTodoCond.wait(lock, [this] { return _workerWorkTodo; });

			_workerWorkTodo = false;
			_workerWorkDone = false;

			if (_terminate)
			{
				break;
			}

			lock.unlock();

			run();
		}
		info("worker done!");
	}
}
