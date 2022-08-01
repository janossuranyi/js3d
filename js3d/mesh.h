#ifndef JS3D_MESH3D_H
#define JS3D_MESH3D_H

#include <vector>
#include <glm/glm.hpp>

namespace js3d {

	class Mesh
	{
	public:
		void add_position(const glm::vec3& v);
		void add_normal(const glm::vec3& v);
		void add_tangent(const glm::vec4& v);
		void add_uv(const glm::vec2& v);
		void add_color(const glm::vec4 v);
		void add_index(const int i);
		void set_bounds(const glm::vec3& min, const glm::vec3& max);
		bool valid() const;
		size_t numVertices() const;
		size_t numIndices() const;
		void reset();
		void get_bounds(glm::vec3& min, glm::vec3& max);

		const glm::vec3* positions() const;
		const glm::vec3* normals() const;
		const glm::vec4* tangents() const;
		const glm::vec2* uvs() const;
		const glm::vec4* colors() const;
		const uint32_t* indices() const;

	private:
		std::vector<glm::vec3> _positions;
		std::vector<glm::vec3> _normals;
		std::vector<glm::vec4> _tangents;
		std::vector<glm::vec2> _uvs;
		std::vector<glm::vec4> _colors;
		std::vector<uint32_t> _indices;

		glm::vec3 _min;
		glm::vec3 _max;
	};
}
#endif // !JS3D_MESH3D_H
