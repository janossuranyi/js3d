#ifndef JS3D_DRAW_VERT_COMP_H
#define JS3D_DRAW_VERT_COMP_H

#include <cinttypes>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define U16_TO_SNORM(x) ((2 * x / 65535.0f) - 1.0f)
#define SNORM_TO_U16(x) ((uint16_t) ((1.0f + x) * 0.5f * 65535.0f))
#define U16_TO_UNORM(x) (x / 65535.0f)
#define UNORM_TO_U16(x) ((uint16_t)(x * 65535.0f))

namespace js3d {

	struct drawVert_t
	{
		float		position[3];	// 12
		uint16_t	normal[3];		//  6
		uint16_t	tangent[4];		//  8
		uint16_t	st[2];			//  4
		uint16_t	user0;			//  2

		void setPosition(const glm::vec3& p0)
		{
			position[0] = p0.x;
			position[1] = p0.y;
			position[2] = p0.z;
		}
		void setNormal(const glm::vec3& p0)
		{
			vec3 p = glm::clamp(p0, -1.0f, 1.0f);
			normal[0] = SNORM_TO_U16(p.x);
			normal[1] = SNORM_TO_U16(p.y);
			normal[2] = SNORM_TO_U16(p.z);
		}
		void setTangent(const glm::vec4& p0)
		{
			vec4 p = glm::clamp(p0, -1.0f, 1.0f);
			tangent[0] = SNORM_TO_U16(p.x);
			tangent[1] = SNORM_TO_U16(p.y);
			tangent[2] = SNORM_TO_U16(p.z);
			tangent[3] = SNORM_TO_U16(p.w);
		}
		void setTextCoord(const glm::vec2& p0)
		{
			vec2 p = glm::clamp(p0, 0.0f, 1.0f);
			st[0] = UNORM_TO_U16(p.x);
			st[1] = UNORM_TO_U16(p.y);
		}
		glm::vec3 getPosition() const
		{
			return glm::make_vec3(position);
		}
		glm::vec3 getNormal()
		{
			return glm::normalize(glm::vec3(
				U16_TO_SNORM(normal[0]),
				U16_TO_SNORM(normal[1]),
				U16_TO_SNORM(normal[2])
			));
		}
		glm::vec4 getTangent() const
		{
			return glm::normalize(glm::vec4(
				U16_TO_UNORM(tangent[0]),
				U16_TO_UNORM(tangent[1]),
				U16_TO_UNORM(tangent[2]),
				U16_TO_UNORM(tangent[3])
			));
		}
		glm::vec2 getTexCoord() const
		{
			return glm::normalize(glm::vec2(
				U16_TO_UNORM(st[0]),
				U16_TO_UNORM(st[1])
			));
		}

	}; // 32
}
#endif // !JS3D_DRAW_VERT_COMP_H
