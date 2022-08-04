#include <cinttypes>
#include <memory>
#include <glm/glm.hpp>
#include "heap.h"
#include "render_mesh.h"
#include "render_system.h"

namespace js3d {
	RenderMesh::RenderMesh()
	{
		_renderSystem = nullptr;
		_vertexCacheHandle = -1;
		_indexCacheHandle = -1;
		_vertices = nullptr;
		_indices = nullptr;
		_numVertices = 0;
		_numIndices = 0;
		_mesh = -1;
		_dataInFrameMemory = true;
	}

	RenderMesh::RenderMesh(int index, const Mesh& mesh, RenderSystem *dm) : RenderMesh()
	{
		
		_renderSystem = dm;
		set_mesh(index, mesh);
	}

	RenderMesh::~RenderMesh()
	{
		purge();
	}

	void RenderMesh::set_mesh(int index, const Mesh& mesh, bool useFrameMemory)
	{
		if (mesh.numVertices() && mesh.valid())
		{
			_mesh = index;

			const uint32_t* elementArray = mesh.indices();

			const glm::vec3* positionArray = mesh.positions();
			const glm::vec3* normalArray = mesh.normals();
			const glm::vec4* tangentArray = mesh.tangents();
			const glm::vec4* colorArray = mesh.colors();
			const glm::vec2* uvArray = mesh.uvs();

			if (useFrameMemory)
			{
				_vertices = static_cast<drawVert_t*>	(_renderSystem->alloc_frame_mem(mesh.numVertices() * sizeof(drawVert_t)));
				_indices = static_cast<elementIndex_t*>	(_renderSystem->alloc_frame_mem(mesh.numIndices() * 2));
			}
			else
			{
				_vertices = new drawVert_t[mesh.numVertices()];
				_indices = new elementIndex_t[mesh.numIndices()];
			}

			memset(_vertices, 0, mesh.numVertices() * sizeof(drawVert_t));
			memset(_indices, 0, mesh.numIndices() * 2);

			_numIndices		= mesh.numIndices();
			_numVertices	= mesh.numVertices();

			for (unsigned i = 0; i < mesh.numVertices(); ++i)
			{
				const glm::vec3 biTangent = glm::cross(normalArray[i], glm::vec3(tangentArray[i]));

				_vertices[i].setPosition(glm::vec4(positionArray[i], 1.0f));
				_vertices[i].setQTangent(normalArray[i], tangentArray[i], biTangent);
				_vertices[i].setTextCoord(uvArray[i]);
				_vertices[i].setColor(colorArray[i]);
			}
			for (unsigned i = 0; i < mesh.numIndices(); ++i)
			{
				_indices[i] = static_cast<elementIndex_t>(elementArray[i]);
			}

		}
	}

	void RenderMesh::set_id(int id)
	{
		_mesh = id;
	}

	int RenderMesh::id() const
	{
		return _mesh;
	}

	uint64_t RenderMesh::indexHandle() const
	{
		return _indexCacheHandle;
	}

	uint64_t RenderMesh::vertexHandle() const
	{
		return _vertexCacheHandle;
	}

	void RenderMesh::set_index_handle(uint64_t h)
	{
		_vertexCacheHandle = h;
	}

	void RenderMesh::set_vertex_handle(uint64_t h)
	{
		_indexCacheHandle = h;
	}

	void RenderMesh::set_vertex_num(int n)
	{
		_numVertices = n;
	}

	void RenderMesh::set_index_num(int n)
	{
		_numIndices = n;
	}

	void RenderMesh::purge()
	{
		if (!_dataInFrameMemory)
		{
			if (_vertices) delete[] _vertices;
			if (_indices) delete[] _indices;
		}
		_vertices = nullptr;
		_indices = nullptr;
	}

}
