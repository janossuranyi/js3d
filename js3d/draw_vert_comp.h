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
#define UNORM_TO_U8(x) ((uint8_t) (x * 255.0f))
#define U8_TO_UNORM(x) ((uint8_t) x / 255.0f)

namespace js3d {

	struct drawVert_t
	{
		float		position[4];	// 16
		uint16_t	qtangent[4];	//  8
		uint16_t	uv[2];			//  4
		uint8_t		color[4];		//  4

		void setPosition(const glm::vec4& p0)
		{
			position[0] = p0.x;
			position[1] = p0.y;
			position[2] = p0.z;
			position[3] = p0.w;
		}
		void setQTangent(const glm::vec4& p0)
		{
			glm::vec4 p = glm::clamp(p0, -1.0f, 1.0f);
			qtangent[0] = SNORM_TO_U16(p.x);
			qtangent[1] = SNORM_TO_U16(p.y);
			qtangent[2] = SNORM_TO_U16(p.z);
			qtangent[2] = SNORM_TO_U16(p.w);
		}
		void setTextCoord(const glm::vec2& p0)
		{
			glm::vec2 p = glm::clamp(p0, 0.0f, 1.0f);
			uv[0] = UNORM_TO_U16(p.x);
			uv[1] = UNORM_TO_U16(p.y);
		}
		void setColor(const glm::vec4& p0)
		{
			glm::vec4 p = glm::clamp(p0, 0.0f, 1.0f);
			color[0] = UNORM_TO_U8(p.r);
			color[1] = UNORM_TO_U8(p.g);
			color[2] = UNORM_TO_U8(p.b);
			color[3] = UNORM_TO_U8(p.a);
		}
		glm::vec4 getPosition() const
		{
			return glm::make_vec4(position);
		}
		glm::vec4 getQTangent() const
		{
			return glm::normalize(glm::vec4(
				U16_TO_UNORM(qtangent[0]),
				U16_TO_UNORM(qtangent[1]),
				U16_TO_UNORM(qtangent[2]),
				U16_TO_UNORM(qtangent[3])
			));
		}
		glm::vec2 getTexCoord() const
		{
			return glm::normalize(glm::vec2(
				U16_TO_UNORM(uv[0]),
				U16_TO_UNORM(uv[1])
			));
		}
	}; // 32
}
#endif // !JS3D_DRAW_VERT_COMP_H
