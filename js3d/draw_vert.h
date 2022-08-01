#ifndef JS3D_DRAW_VERT_COMP_H
#define JS3D_DRAW_VERT_COMP_H

#include <cinttypes>
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "math_converters.h"

#undef _glm
#undef _std
#define _glm glm::
#define _std std::

namespace js3d {

	struct drawVert_t
	{
		halfFloat_t	position[4];	//  8
		uint16_t	qtangent[4];	//  8
		halfFloat_t	uv[2];			//  4
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
				const float normFactor = _std sqrtf(1.0f - bias * bias);
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
			position[0] = F32toF16(p0.x);
			position[1] = F32toF16(p0.y);
			position[2] = F32toF16(p0.z);
			position[3] = F32toF16(p0.w);
		}
		void setQTangent(const _glm quat& p0)
		{
			qtangent[0] = F32toF16(p0.x);
			qtangent[1] = F32toF16(p0.y);
			qtangent[2] = F32toF16(p0.z);
			qtangent[3] = F32toF16(p0.w);
		}
		void setTextCoord(const _glm vec2& p0)
		{
			uv[0] = F32toF16(p0.x);
			uv[1] = F32toF16(p0.y);
		}
		void setColor(const _glm vec4& p0)
		{
			glm::vec4 p = _glm clamp(p0, 0.0f, 1.0f);
			color[0] = floatToUnorm8(p.r);
			color[1] = floatToUnorm8(p.g);
			color[2] = floatToUnorm8(p.b);
			color[3] = floatToUnorm8(p.a);
		}
		_glm vec4 getPosition() const
		{
			return _glm vec4(
				F16toF32(position[0]),
				F16toF32(position[1]),
				F16toF32(position[2]),
				F16toF32(position[3])
			);
		}
		_glm vec4 getQTangent() const
		{
			return _glm normalize(_glm vec4(
				snorm16ToFloat(qtangent[0]),
				snorm16ToFloat(qtangent[1]),
				snorm16ToFloat(qtangent[2]),
				snorm16ToFloat(qtangent[3])
			));
		}
		_glm vec2 getTexCoord() const
		{
			return _glm normalize(_glm vec2(
				F16toF32(uv[0]),
				F16toF32(uv[1])
			));
		}
		glm::vec4 getColor() const
		{
			return _glm normalize(_glm vec4(
				unorm8ToFloat(color[0]),
				unorm8ToFloat(color[1]),
				unorm8ToFloat(color[2]),
				unorm8ToFloat(color[3])
			));

		}
	}; // 24
}
#endif // !JS3D_DRAW_VERT_COMP_H
