#include <SDL.h>
#include "vertex_cache.h"
#include "logger.h"

#define JS3D_ALIGN16(x) (((x) + 15) & ~15)
#define JS3D_ALIGN32(x) (((x) + 31) & ~31)

namespace js3d {

	VertexCache::VertexCache()
	{
		_currentFrame = 0;
		_listNum = 0;
		_drawListNum = 0;
		_static_cache.allocations = 0;
		_static_cache.indexMemUsed = 0;
		_static_cache.vertexMemUsed = 0;
		_static_cache.mappedIndexBase = nullptr;
		_static_cache.mappedVertexBase = nullptr;
	}

	VertexCache::~VertexCache() noexcept
	{

	}

	void VertexCache::init()
	{
		_static_cache.vertexBuffer.init(eBufferUsage::STATIC, VERTCACHE_SIZE, nullptr);
		_static_cache.indexBuffer.init(eBufferUsage::STATIC, VERTCACHE_SIZE, nullptr);
		_static_cache.mappedVertexBase = nullptr;
		_static_cache.mappedIndexBase = nullptr;

		for (int i = 0; i < NUM_FRAME_DATA; ++i)
		{
			_frame_data[i].allocations = 0;
			_frame_data[i].indexMemUsed = 0;
			_frame_data[i].vertexMemUsed = 0;
			_frame_data[i].indexBuffer.init(eBufferUsage::DYNAMIC, VERTCACHE_INDEX_MEMORY_PER_FRAME, nullptr);
			_frame_data[i].vertexBuffer.init(eBufferUsage::DYNAMIC, VERTCACHE_VERTEX_MEMORY_PER_FRAME, nullptr);
			_frame_data[i].mappedIndexBase = nullptr;
			_frame_data[i].mappedVertexBase = nullptr;
		}

		_listNum = 0;
		_drawListNum = 1;

		map_buffer_set(_frame_data[_listNum]);
	}

	vertCacheHandle_t VertexCache::alloc_vertex(const void* data, int num, size_t size)
	{
		vertCacheHandle_t handle = internal_alloc(_frame_data[_listNum], data, JS3D_ALIGN32( num * size ), CACHE_VERTEX);

		return handle;
	}

	vertCacheHandle_t VertexCache::alloc_index(const void* data, int num, size_t size)
	{
		vertCacheHandle_t handle = internal_alloc(_frame_data[_listNum], data, JS3D_ALIGN16( size * num ), CACHE_INDEX);

		return handle;
	}

	vertCacheHandle_t VertexCache::alloc_static_vertex(const void* data, int num, size_t size)
	{
		vertCacheHandle_t handle = internal_alloc(_static_cache, data, JS3D_ALIGN32( size * num ), CACHE_VERTEX);

		return handle;
	}

	vertCacheHandle_t VertexCache::alloc_static_index(const void* data, int num, size_t size)
	{
		vertCacheHandle_t handle = internal_alloc(_static_cache, data, JS3D_ALIGN16( size * num ), CACHE_INDEX);

		return handle;
	}

	void VertexCache::begin_backend()
	{
		_frame_data[_listNum].indexMemUsed = 0;
		_frame_data[_listNum].vertexMemUsed = 0;
		_frame_data[_listNum].allocations = 0;

		uint64_t start = SDL_GetTicks64();

		unmap_buffer_set(_frame_data[_listNum]);
		unmap_buffer_set(_static_cache);

		uint64_t end = SDL_GetTicks64();

		if ((end - start) > 1)
			info("un-map buffers time: %d ms", (end - start));

		_drawListNum = _listNum;

		_currentFrame++;
		_listNum = _currentFrame % NUM_FRAME_DATA;

		start = SDL_GetTicks64();
		map_buffer_set(_frame_data[_listNum]);
		end = SDL_GetTicks64();

		if ((end - start) > 1)
			info("map buffers time: %d ms", (end - start));

	}

	bool VertexCache::decode_handle(vertCacheHandle_t handle, uint32_t& offset, uint32_t& size, bool& isStatic)
	{
		isStatic = handle & VERTCACHE_STATIC;
		offset = (handle >> VERTCACHE_OFFSET_SHIFT) & VERTCACHE_OFFSET_MASK;
		size = (handle >> VERTCACHE_SIZE_SHIFT) & VERTCACHE_SIZE_MASK;

		if ( ! isStatic )
		{
			int frameNum = (handle >> VERTCACHE_FRAME_SHIFT) & VERTCACHE_FRAME_MASK;
			return frameNum == ((_currentFrame - 1) & VERTCACHE_FRAME_MASK);
		}

		return true;
	}

	vertCacheHandle_t VertexCache::internal_alloc(geoBufferSet_t& set, const void* data, int bytes, cacheType_t type)
	{

		assert(data != nullptr);
		assert((((uintptr_t)(data)) & 15) == 0);

		uint32_t offset = 0;
		uint32_t endPos = 0;

		if (type == CACHE_VERTEX)
		{
			offset = set.vertexMemUsed.fetch_add(bytes);
			endPos = offset + bytes;
			if (set.vertexBuffer.size() < endPos)
			{
				error("Out of vertex cache!");
			}
			if (data) { 
				set.vertexBuffer.update_data(offset, bytes, data); 
			}
		}
		else if (type == CACHE_INDEX)
		{
			offset = set.indexMemUsed.fetch_add(bytes);
			endPos = offset + bytes;
			if (set.indexBuffer.size() < endPos)
			{
				error("Out of index cache!");
			}
			if (data)
			{
				set.indexBuffer.update_data(offset, bytes, data);
			}
				
		}

		vertCacheHandle_t h = CREATE_VERTCACHE_HANDLE(_currentFrame, offset, bytes);
		set.allocations++;

		if (&set == &_static_cache)
		{
			h |= VERTCACHE_STATIC;
		}

		return h;
	}

	void VertexCache::map_buffer_set(geoBufferSet_t& set)
	{
		if (set.mappedVertexBase == nullptr)
		{
			set.mappedVertexBase = set.vertexBuffer.map_write();
		}
		if (set.mappedIndexBase == nullptr)
		{
			set.mappedIndexBase = set.indexBuffer.map_write();
		}
	}

	void VertexCache::unmap_buffer_set(geoBufferSet_t& set)
	{
		if (set.mappedVertexBase != nullptr)
		{
			set.vertexBuffer.unmap();
			set.mappedVertexBase = nullptr;
		}
		if (set.mappedIndexBase != nullptr)
		{
			set.indexBuffer.unmap();
			set.mappedIndexBase = nullptr;
		}
	}

	VertexCache g_vertexCache;
}
