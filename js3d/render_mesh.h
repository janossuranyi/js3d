#ifndef JS3D_RENDER_MESH_H
#define JS3D_RENDER_MESH_H

#include <vector>
#include "draw_vert.h"
#include "mesh.h"

namespace js3d {

	class RenderSystem;
	
	class RenderMesh
	{
	public:
		RenderMesh();
		RenderMesh(int index, const Mesh& mesh, RenderSystem* dm);
		void set_mesh(int index, const Mesh& mesh);
		void set_id(int id);
		int id() const;
		uint64_t indexHandle() const;
		uint64_t vertexHandle() const;
		int numVertices() const { return _numVertices; };
		int numIndices() const { return _numIndices; }
		void set_index_handle(uint64_t h);
		void set_vertex_handle(uint64_t h);
		void set_vertex_num(int n);
		void set_index_num(int n);
		const drawVert_t* vertexArray() const { return _vertices; }
		const uint16_t* elementArray() const { return _indices; }

		void purge();

	private:
		int _mesh;
		uint64_t _indexCacheHandle;
		uint64_t _vertexCacheHandle;

		int _numVertices;
		int _numIndices;

		drawVert_t* _vertices;
		uint16_t* _indices;

		RenderSystem* _renderSystem;
	};
}
#endif // !JS3D_RENDER_MESH_H
