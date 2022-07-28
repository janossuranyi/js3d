#ifndef JS3D_DRAW_SURF_H
#define JS3D_DRAW_SURF_H

#include "render_common.h"
#include "tri_surf.h"
#include "texture.h"

namespace js3d {
	struct drawSurface_t
	{
		eDrawElementType elementType;
		triSurf_t* vertexData;
		Texture* diffuse;
		// material_t* material;
	};

}
#endif // !JS3D_DRAW_SURF_H
