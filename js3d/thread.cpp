
#include <SDL.h>
#include "thread.h"

namespace js3d {
	
	int Thread_threadProc(void* context);

	Thread::Thread(const char* name)
	{
		_name = name;
		_ret = 0;
		_workerWorkDone = SDL_CreateSemaphore(0);
		_workerFireWork = SDL_CreateSemaphore(0);
		SDL_AtomicSet(&_terminate, 0);
		_thread = SDL_CreateThread(Thread_threadProc, name, this);
	}

	Thread::~Thread() noexcept
	{
		int res;

		SDL_AtomicSet(&_terminate, 1);
		SDL_SemPost(_workerFireWork);
		SDL_WaitThread(_thread, &res);

		SDL_DestroySemaphore(_workerFireWork);
		SDL_DestroySemaphore(_workerWorkDone);

		SDL_Log("Thread %s exited", _name);
	}

	int Thread::start_worker()
	{

		SDL_SemWait(_workerWorkDone);

		int latch = _ret;

		SDL_SemPost(_workerFireWork);

		return _ret;
	}

	void Thread::wait_for_done()
	{
		SDL_SemWait(_workerWorkDone);
	}

	void Thread::run()
	{
		_ret = 1;
	}

	static int Thread_threadProc(void* context)
	{
		Thread* _this = reinterpret_cast<Thread*>(context);

		while (!SDL_AtomicGet(&_this->_terminate))
		{
			SDL_SemPost(_this->_workerWorkDone);
			SDL_SemWait(_this->_workerFireWork);

			if (SDL_AtomicGet(&_this->_terminate))
				break;

			_this->run();
		}

		return 0;
	}

}
