#pragma once
#ifndef JS3D_RENDER_COMMON_H
#define JS3D_RENDER_COMMON_H

#define BIT( n ) (1 << n)
#define MASK( n ) ( ( 1 << n ) - 1)

#define CLEAR_FB_COLOR (1<<0)
#define CLEAR_FB_DEPTH (1<<1)
#define CLEAR_FB_STENCIL (1<<2)

#define NUM_FRAME_DATA (2)

#include <cinttypes>

namespace js3d {

	class RenderMesh;
	struct drawSurface_t;
	
	enum class eDrawElementType { POINT, LINES, TRIANGLES };
	enum vertexLayout_t { VERTEX_LAYOUT_DRAW_VERT, VERTEX_LAYOUT_LAST_ENUM };

	const int FRAME_DATA_SIZE = 127 * 1024 * 1024;
	const int FRAME_DATA_MEM_ALIGN = 128;

	//enum class vertexLayoutType_t { FULL, COMPACT, SHADOW };

	typedef uint16_t elementIndex_t;
	typedef uint64_t vertCacheHandle_t;

	enum shaderType_t  {
		VERTEX_SHADER,
		FRAGMENT_SHADER,
		GEOMETRY_SHADER,
		COMPUTE_SHADER
	};

	enum renderCommand_t
	{
		RC_NOP,
		RC_CREATE_MESH,
		RC_REMOVE_MESH,
		RC_DRAW_SURF
	};

	struct emptyCommand_t
	{
		renderCommand_t commandId;
		renderCommand_t* next;
	};

	struct createMeshCommand_t
	{
		renderCommand_t commandId;
		renderCommand_t* next;
		RenderMesh* r_mesh;
	};

	struct removeMeshCommand_t
	{
		renderCommand_t commandId;
		renderCommand_t* next;
		int id;
	};

	struct drawSurfaceCommand_t
	{
		renderCommand_t commandId;
		renderCommand_t* next;
		drawSurface_t* drawSurf;
	};

}
#endif // !JS3D_RENDER_COMMON_H
