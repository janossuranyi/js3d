#ifndef JS3D_DRAW_VERT_H
#define JS3D_DRAW_VERT_H

#error use drawVertCompact

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "render_common.h"

namespace js3d {

	struct drawVert_t
	{
		float		position[3];	// 12
		float		normal[3];		// 12
		float		tangent[3];		// 12
		float		st[2];			//  8
		uint16_t	user[2];		//  4

		void setTexCoord(const glm::vec2& p0)
		{
			st[0] = p0[0];
			st[1] = p0[1];
		}
		void setPosition(const glm::vec3& p0)
		{
			position[0] = p0[0];
			position[1] = p0[1];
			position[2] = p0[2];
		}
		void setNormal(const glm::vec3& p0)
		{
			normal[0] = p0[0];
			normal[1] = p0[1];
			normal[2] = p0[2];
		}
		void setTangent(const glm::vec3& p0)
		{
			tangent[0] = p0[0];
			tangent[1] = p0[1];
			tangent[2] = p0[2];
		}
		glm::vec3 getPosition() const
		{
			return glm::make_vec3(position);
		}
		glm::vec3 getNormal()
		{
			return glm::make_vec3(normal);
		}
		glm::vec3 getTangent() const
		{
			return glm::make_vec3(tangent);
		}
		glm::vec2 getTexCoord() const
		{
			return glm::make_vec2(st);
		}
	}; // 48

}
#endif // !JS3D_DRAW_VERT_H
