#ifndef JS3D_VERTEX_CACHE_H
#define JS3D_VERTEX_CACHE_H

#include <cinttypes>
#include <atomic>
#include "render_system.h"
#include "render_common.h"
#include "buffer.h"
#include "draw_vert.h"

#define CREATE_VERTCACHE_HANDLE(frameNum,offset,bytes) \
	( (uint64_t) (frameNum & VERTCACHE_FRAME_MASK) << VERTCACHE_FRAME_SHIFT) | \
	( (uint64_t) (offset & VERTCACHE_OFFSET_MASK) << VERTCACHE_OFFSET_SHIFT) | \
	( (uint64_t) (bytes & VERTCACHE_SIZE_MASK) << VERTCACHE_SIZE_SHIFT)

namespace js3d {

	const int VERTCACHE_INDEX_MEMORY_PER_FRAME = 31 * 1024 * 1024;
	const int VERTCACHE_VERTEX_MEMORY_PER_FRAME = 31 * 1024 * 1024;

	const int VERTCACHE_SIZE			= 256 * 1024 * 1024;
	const int VERTCACHE_STATIC			= 1;
	const int VERTCACHE_SIZE_SHIFT		= 1;
	const int VERTCACHE_SIZE_MASK		= (1 << 24) - 1;  // 24 bit = 16MB
	const int VERTCACHE_OFFSET_SHIFT	= 25;
	const int VERTCACHE_OFFSET_MASK		= (1 << 28) - 1;  // 28 bit = 256MB
	const int VERTCACHE_FRAME_SHIFT		= 53;
	const int VERTCACHE_FRAME_MASK		= (1 << 11) - 1;  // 11 bit = 2048

	enum cacheType_t
	{
		CACHE_VERTEX,
		CACHE_INDEX
	};

	struct geoBufferSet_t
	{
		IndexBuffer			indexBuffer;
		VertexBuffer		vertexBuffer;
		std::atomic_uint	indexMemUsed;
		std::atomic_uint	vertexMemUsed;
		uint8_t*			mappedVertexBase;
		uint8_t*			mappedIndexBase;
		int					allocations;	// number of index and vertex allocations combined
	};

	class VertexCache
	{
		friend class RenderSystem;
	public:
		VertexCache();
		~VertexCache() noexcept;

		void init();

		vertCacheHandle_t alloc_vertex(const void* data, int num, size_t size = sizeof(drawVert_t));
		vertCacheHandle_t alloc_index(const void* data, int num, size_t size = sizeof(elementIndex_t));
		vertCacheHandle_t alloc_static_vertex(const void* data, int num, size_t size = sizeof(drawVert_t));
		vertCacheHandle_t alloc_static_index(const void* data, int num, size_t size = sizeof(elementIndex_t));

		void begin_backend();

		bool decode_handle(vertCacheHandle_t handle, uint32_t& offset, uint32_t& size, bool& isStatic);
		bool get_vertex(vertCacheHandle_t handle, VertexBuffer& ref);
		bool get_index(vertCacheHandle_t handle, IndexBuffer& ref);
	private:
		vertCacheHandle_t internal_alloc(geoBufferSet_t& set, const void* data, int bytes, cacheType_t type);

		geoBufferSet_t _static_cache;
		geoBufferSet_t _frame_data[NUM_FRAME_DATA];

		void map_buffer_set(geoBufferSet_t& set);
		void unmap_buffer_set(geoBufferSet_t& set);

		int _currentFrame;
		int _listNum, _drawListNum;
	};

	extern VertexCache g_vertexCache;
}
#endif // !JS3D_VERTEX_CACHE_H
