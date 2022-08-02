#ifndef JS3D_THREAD_H
#define JS3D_THREAD_H

#include <thread>
#include <mutex>

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
		void threadProc();

		virtual void run();

	protected:
		void set_result(int r) { _ret = r; }
	private:
		bool _terminate;
		bool _workerWorkDone;
		bool _workerWorkTodo;
		std::condition_variable _workerWorkDone_cv;
		std::condition_variable _workerWorkTodo_cv;
		std::mutex _mtx;
		std::thread _thread;
		const char* _name;
		int _ret;
	};
}
#endif // !JS3D_THREAD_H
