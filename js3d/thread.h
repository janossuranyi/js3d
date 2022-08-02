#ifndef JS3D_THREAD_H
#define JS3D_THREAD_H

#include <SDL.h>

namespace js3d {

	//typedef int(threadProc_t*)(void*);

	class Thread
	{

	public:
		Thread(const char* name);
		~Thread() noexcept;
		int result() const { return _ret; }
		const char* name() const { return _name; }
		int start_worker();
		void wait_for_done();
		friend int Thread_threadProc(void*);

		virtual void run();

	protected:
		void set_result(int r) { _ret = r; }
	private:
		SDL_sem* _workerWorkDone;
		SDL_sem* _workerFireWork;
		SDL_Thread* _thread;
		SDL_atomic_t _terminate;

		const char* _name;
		int _ret;
	};
}
#endif // !JS3D_THREAD_H
