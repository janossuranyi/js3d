#ifndef DEMO_RENDER_WORKER_H
#define DEMO_RENDER_WORKER_H

#include "thread.h"

class MyWorker : public js3d::Worker
{
public:
	const float K1 = 1.0f;
	const float K0 = 0.0f;

	MyWorker(const char* name) : Worker(name)
	{
		surf.meshId = 0;
		surf.material = &mat;
		t1 = t2 = t3 = -1;
	}
	void run() override;
	void init();

	js3d::drawSurface_t surf;
	js3d::Material mat;
	bool _first_run{ true };
	js3d::Mesh rect;
	int t1, t2, t3;


};

#endif // !DEMO_RENDER_WORKER_H
