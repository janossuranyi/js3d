#ifndef JS3D_DRAW_SURF_H
#define JS3D_DRAW_SURF_H

#include "render_common.h"
#include "tri_surf.h"
#include "texture.h"
#include "material.h"
#include "shader_manager.h"
#include "GLState.h"

namespace js3d {
	struct drawSurface_t
	{
		drawSurface_t() :
			elementType(eDrawElementType::TRIANGLES),
			material(nullptr),
			vertexLayout(VERTEX_LAYOUT_DRAW_VERT),
			stateFlags(GLS_CULL_TWOSIDED | GLS_DEPTHFUNC_ALWAYS),
			receiveShadow(true),
			castShadow(true),
			meshId(-1),
			vertexData()
		{}

		eDrawElementType elementType;
		triSurf_t* vertexData;
		int meshId;
		Material* material;
		vertexLayout_t vertexLayout;
		uint64_t stateFlags;
		bool receiveShadow;
		bool castShadow;
	};

}
#endif // !JS3D_DRAW_SURF_H
