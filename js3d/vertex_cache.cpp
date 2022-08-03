#include "vertex_cache.h"
#include "logger.h"

#define JS3D_ALIGN16(x) (((x) + 15) & ~15)

namespace js3d {

	VertexCache::VertexCache()
	{
		_currentFrame = 0;
		_listNum = 0;
		_drawListNum = 0;
		_static_cache.allocations = 0;
		_static_cache.indexMemUsed = 0;
		_static_cache.vertexMemUsed = 0;
	}

	VertexCache::~VertexCache() noexcept
	{

	}

	void VertexCache::init()
	{
		_static_cache.vertexBuffer.init(eBufferUsage::STATIC, VERTCACHE_SIZE, nullptr);
		_static_cache.indexBuffer.init(eBufferUsage::STATIC, VERTCACHE_SIZE, nullptr);
		for (int i = 0; i < NUM_FRAME_DATA; ++i)
		{
			_frame_data[i].allocations = 0;
			_frame_data[i].indexMemUsed = 0;
			_frame_data[i].vertexMemUsed = 0;
			_frame_data[i].indexBuffer.init(eBufferUsage::DYNAMIC, VERTCACHE_INDEX_MEMORY_PER_FRAME, nullptr);
			_frame_data[i].vertexBuffer.init(eBufferUsage::DYNAMIC, VERTCACHE_VERTEX_MEMORY_PER_FRAME, nullptr);
		}
	}

	vertCacheHandle_t VertexCache::alloc_vertex(const void* data, int num, size_t size)
	{
		vertCacheHandle_t handle = internal_alloc(_frame_data[_listNum], data, num * size, CACHE_VERTEX);

		return handle;
	}

	vertCacheHandle_t VertexCache::alloc_index(const void* data, int num, size_t size)
	{
		vertCacheHandle_t handle = internal_alloc(_frame_data[_listNum], data, size * num, CACHE_INDEX);

		return handle;
	}

	vertCacheHandle_t VertexCache::alloc_static_vertex(const void* data, int num, size_t size)
	{
		vertCacheHandle_t handle = internal_alloc(_static_cache, data, size * num, CACHE_VERTEX);

		return handle;
	}

	vertCacheHandle_t VertexCache::alloc_static_index(const void* data, int num, size_t size)
	{
		vertCacheHandle_t handle = internal_alloc(_static_cache, data, size * num, CACHE_INDEX);

		return handle;
	}

	void VertexCache::begin_backend()
	{
		_drawListNum = _listNum;
		_currentFrame++;
		_listNum = _currentFrame % NUM_FRAME_DATA;
		_frame_data[_listNum].indexMemUsed = 0;
		_frame_data[_listNum].vertexMemUsed = 0;
		_frame_data[_listNum].allocations = 0;
	}

	bool VertexCache::decode_handle(vertCacheHandle_t handle, uint32_t& offset, uint32_t& size, bool& isStatic)
	{
		isStatic = handle & VERTCACHE_STATIC;
		offset = (handle >> VERTCACHE_OFFSET_SHIFT) & VERTCACHE_OFFSET_MASK;
		size = (handle >> VERTCACHE_SIZE_SHIFT) & VERTCACHE_SIZE_MASK;

		if ( ! isStatic )
		{
			int frameNum = (handle >> VERTCACHE_FRAME_SHIFT) & VERTCACHE_FRAME_MASK;
			return frameNum == _currentFrame;
		}

		return true;
	}

	vertCacheHandle_t VertexCache::internal_alloc(geoBufferSet_t& set, const void* data, int bytes, cacheType_t type)
	{

		assert(data != nullptr);
		assert((((uintptr_t)(data)) & 15) == 0);

		uint32_t alignedSize = JS3D_ALIGN16(bytes);

		uint32_t offset = 0;
		uint32_t endPos = 0;

		if (type == CACHE_VERTEX)
		{
			offset = set.vertexMemUsed.fetch_add(alignedSize);
			endPos = offset + alignedSize;
			if (set.vertexBuffer.size() < endPos)
			{
				error("Out of vertex cache!");
			}
			set.vertexBuffer.update_data(offset, bytes, data);
		}
		else if (type == CACHE_INDEX)
		{
			offset = set.indexMemUsed.fetch_add(alignedSize);
			endPos = offset + alignedSize;
			if (set.indexBuffer.size() < endPos)
			{
				error("Out of index cache!");
			}
			set.indexBuffer.update_data(offset, bytes, data);
		}

		vertCacheHandle_t h = CREATE_VERTCACHE_HANDLE(_currentFrame, offset, alignedSize);
		set.allocations++;

		if (&set == &_static_cache)
		{
			h |= VERTCACHE_STATIC;
		}

		return h;
	}

	VertexCache g_vertexCache;
}
