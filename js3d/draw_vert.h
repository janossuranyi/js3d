#ifndef JS3D_DRAW_VERT_COMP_H
#define JS3D_DRAW_VERT_COMP_H

#include <cinttypes>
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "math_vector.h"

#define U16_TO_SNORM(x) ((2 * x / 65535.0f) - 1.0f)
#define SNORM_TO_U16(x) ((uint16_t) ((1.0f + x) * 0.5f * 65535.0f))
#define U16_TO_UNORM(x) (x / 65535.0f)
#define UNORM_TO_U16(x) ((uint16_t)(x * 65535.0f))
#define UNORM_TO_U8(x) ((uint8_t) (x * 255.0f))
#define U8_TO_UNORM(x) (x / 255.0f)

#undef _glm
#define _glm glm::

namespace js3d {

	struct drawVert_t
	{
		float		position[4];	// 16
		uint16_t	qtangent[4];	//  8
		uint16_t	uv[2];			//  4
		uint8_t		color[4];		//  4

		void setQTangent(const _glm vec3& normal, const _glm vec3& tangent, const _glm vec3& biTangent)
		{
			_glm mat3 tbn;
			tbn[0] = normal;
			tbn[1] = tangent;
			tbn[2] = biTangent;

			_glm quat qTangent(tbn);
			_glm normalize(qTangent);
			if (qTangent.w < 0)
			{
				qTangent = -qTangent;
			}

			const float bias = 1.0f / 32767.0f;

			if (qTangent.w < bias)
			{
				const float normFactor = std::sqrtf(1.0f - bias * bias);
				qTangent.w = bias;
				qTangent.x *= normFactor;
				qTangent.y *= normFactor;
				qTangent.z *= normFactor;
			}

			_glm vec3 naturalBinormal = _glm cross(tangent, normal);
			if (_glm dot(naturalBinormal, biTangent) <= 0)
			{
				qTangent = -qTangent;
			}

			setQTangent(qTangent);
		}

		void setPosition(const _glm vec4& p0)
		{
			position[0] = p0.x;
			position[1] = p0.y;
			position[2] = p0.z;
			position[3] = p0.w;
		}
		void setQTangent(const _glm quat& p0)
		{
			qtangent[0] = floatToSnorm16(p0.x);
			qtangent[1] = floatToSnorm16(p0.y);
			qtangent[2] = floatToSnorm16(p0.z);
			qtangent[3] = floatToSnorm16(p0.w);
		}
		void setTextCoord(const _glm vec2& p0)
		{
			glm::vec2 p = _glm clamp(p0, 0.0f, 1.0f);
			uv[0] = UNORM_TO_U16(p.x);
			uv[1] = UNORM_TO_U16(p.y);
		}
		void setColor(const _glm vec4& p0)
		{
			glm::vec4 p = _glm clamp(p0, 0.0f, 1.0f);
			color[0] = UNORM_TO_U8(p.r);
			color[1] = UNORM_TO_U8(p.g);
			color[2] = UNORM_TO_U8(p.b);
			color[3] = UNORM_TO_U8(p.a);
		}
		_glm vec4 getPosition() const
		{
			return _glm make_vec4(position);
		}
		_glm vec4 getQTangent() const
		{
			return _glm normalize(_glm vec4(
				U16_TO_UNORM(qtangent[0]),
				U16_TO_UNORM(qtangent[1]),
				U16_TO_UNORM(qtangent[2]),
				U16_TO_UNORM(qtangent[3])
			));
		}
		_glm vec2 getTexCoord() const
		{
			return glm::normalize(_glm vec2(
				U16_TO_UNORM(uv[0]),
				U16_TO_UNORM(uv[1])
			));
		}
		glm::vec4 getColor() const
		{
			return _glm normalize(_glm vec4(
				U8_TO_UNORM(color[0]),
				U8_TO_UNORM(color[1]),
				U8_TO_UNORM(color[2]),
				U8_TO_UNORM(color[3])
			));

		}
	}; // 32
}
#endif // !JS3D_DRAW_VERT_COMP_H
