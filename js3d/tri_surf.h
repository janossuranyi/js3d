#ifndef JS3D_TRI_SURF_H
#define JS3D_TRI_SURF_H

#include "render_common.h"

namespace js3d {

	struct drawVert_t;

	struct triSurf_t
	{
		triSurf_t() :
			vertices(nullptr),
			indices(nullptr),
			numIndices(0),
			numVerts(0),
			vertHandle(0),
			idxHandle(0) {}

		drawVert_t* vertices;
		elementIndex_t* indices;
		int numIndices;
		int numVerts;
		vertCacheHandle_t vertHandle;
		vertCacheHandle_t idxHandle;

	};
}
#endif // !JS3D_TRI_SURF_H
