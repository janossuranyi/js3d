#include "mesh.h"

namespace js3d {
	void Mesh::add_position(const glm::vec3& v)
	{
		_positions.push_back(v);
	}
	void Mesh::add_normal(const glm::vec3& v)
	{
		_normals.push_back(v);
	}
	void Mesh::add_tangent(const glm::vec4& v)
	{
		_tangents.push_back(v);
	}
	void Mesh::add_uv(const glm::vec2& v)
	{
		_uvs.push_back(v);
	}
	void Mesh::add_color(const glm::vec4 v)
	{
		_colors.push_back(v);
	}

	void Mesh::add_index(const int i)
	{
		_indices.push_back(i);
	}

	void Mesh::set_bounds(const glm::vec3& min, const glm::vec3& max)
	{
		_min = min;
		_max = max;
	}

	bool Mesh::valid() const
	{
		size_t psz = _positions.size();
		size_t nsz = _normals.size();
		size_t tsz = _tangents.size();
		size_t usz = _uvs.size();
		size_t csz = _colors.size();

		return (psz == nsz && psz == tsz && psz == usz && psz == csz);
	}
	size_t Mesh::numVertices() const
	{
		return _positions.size();
	}
	size_t Mesh::numIndices() const
	{
		return _indices.size();
	}
	void Mesh::reset()
	{
		_positions.resize(0);
		_normals.resize(0);
		_tangents.resize(0);
		_uvs.resize(0);
		_colors.resize(0);
	}
	void Mesh::get_bounds(glm::vec3& min, glm::vec3& max)
	{
		min = _min;
		max = _max;
	}
	const glm::vec3* Mesh::positions() const
	{
		return _positions.data();
	}
	const glm::vec3* Mesh::normals() const
	{
		return _normals.data();
	}
	const glm::vec4* Mesh::tangents() const
	{
		return _tangents.data();
	}
	const glm::vec2* Mesh::uvs() const
	{
		return _uvs.data();
	}
	const glm::vec4* Mesh::colors() const
	{
		return _colors.data();
	}
	const uint32_t* Mesh::indices() const
	{
		return _indices.data();
	}
}