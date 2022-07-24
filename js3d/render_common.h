#pragma once
#ifndef JS3D_RENDER_COMMON_H
#define JS3D_RENDER_COMMON_H

#define BIT( n ) (1 << n)
#define MASK( n ) ( ( 1 << n ) - 1)

#define CLEAR_FB_COLOR (1<<0)
#define CLEAR_FB_DEPTH (1<<1)
#define CLEAR_FB_STENCIL (1<<2)

#define NUM_FRAME_DATA 2

#include <cinttypes>

namespace js3d {
	enum class eDrawElementType { POINT, LINES, TRIANGLES };
	enum class eVertexLayout { DRAW_VERT, SHADOW_VERT, UNDEFINED };

	enum class eVertexLayoutType { FULL, COMPACT, SHADOW };

	typedef uint16_t elementIndex_t;
	typedef uint64_t vertCacheHandle_t;

	const uint64_t GLS_CULL_FACE = (1 << 0);
	const uint64_t GLS_CULL_BACK = (0 << 1);
	const uint64_t GLS_CULL_FRONT = (1 << 1);
	const uint64_t GLS_CULL_BITS = (GLS_CULL_FRONT | GLS_CULL_BACK);
	const uint64_t GLS_POLYGON_MODE_FILL = (0 << 2);
	const uint64_t GLS_POLYGON_MODE_LINE = (1 << 2);
	const uint64_t GLS_POLYGON_MODE_BITS = GLS_POLYGON_MODE_LINE;
	const uint64_t GLS_DEPTH_FUNC_NEVER = (0 << 3);
	const uint64_t GLS_DEPTH_FUNC_LESS = (1 << 3);
	const uint64_t GLS_DEPTH_FUNC_EQUAL = (2 << 3);
	const uint64_t GLS_DEPTH_FUNC_LEQUAL = (3 << 3);
	const uint64_t GLS_DEPTH_FUNC_GREATER = (4 << 3);
	const uint64_t GLS_DEPTH_FUNC_NOTEQUAL = (5 << 3);
	const uint64_t GLS_DEPTH_FUNC_GEQUAL = (6 << 3);
	const uint64_t GLS_DEPTH_FUNC_ALLWAYS = (7 << 3);
	const uint64_t GLS_DEPTH_FUNC_BITS = (7 << 3);
	const uint64_t GLS_DEPTH_MASK = (1 << 6);

}
#endif // !JS3D_RENDER_COMMON_H
