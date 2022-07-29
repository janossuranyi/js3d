#ifndef JS3D_DRAW_SURF_H
#define JS3D_DRAW_SURF_H

#include "render_common.h"
#include "tri_surf.h"
#include "texture.h"
#include "shader_manager.h"

namespace js3d {
	struct drawSurface_t
	{
		eDrawElementType elementType;
		triSurf_t* vertexData;
		Texture* albedo;
		Texture* metallic_roughness;
		Texture* normal;
		ShaderProgram* shader;
		// material_t* material;
	};

}
#endif // !JS3D_DRAW_SURF_H
