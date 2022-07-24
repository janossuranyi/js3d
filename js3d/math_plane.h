#ifndef JS3D_MATH_PLANE_H
#define JS3D_MATH_PLANE_H

#include <cmath>

#include "math_vector.h"

namespace js3d {

	const float EPSILON = 1e-6f;

	struct Plane
	{
		vec3 N;
		float D;

		Plane(const vec3& normal_, float distance_) : N(normal_), D(distance_) {}
		Plane(float x_, float y_, float z_, float distance_) : N(vec3(x_,y_,z_)), D(distance_){}
		Plane(const vec3& p_)
		{
			D = length(p_);
			N = p_ / D;
		}
		Plane(const vec3& p1, const vec3& p2, const vec3& p3)
		{
			vec3 p12	= p2 - p1;
			vec3 p13	= p3 - p1;
			vec3 n		= cross(p12, p13);

			D = length(n);
			N = normalize(n);
		}

		const vec3& normal() const { return N; }
		float distance() const { return D; }
		float point_distance(const vec3& V) { return dot(V, N) - D; }
		bool intersect_ray(const vec3& l0, const vec3& l, vec3& intersect, float& t);
		bool intersect_line(const vec3& p1, const vec3& p2, vec3& intersect, float& t);
	};

	inline bool Plane::intersect_line(const vec3& p1, const vec3& p2, vec3& intersect, float& t)
	{
		float d1 = point_distance(p1);
		float d2 = point_distance(p2);
		
		if (d1 > 0 && d2 > 0)
		{
			t = 1.0f;
			return false;
		}
			
		if (d1 < 0 && d2 < 0)
		{
			t = -1.0f;
			return false;
		}

		return intersect_ray(p1, normalize(p2 - p1), intersect, t);
	}

	inline bool Plane::intersect_ray(const vec3& l0, const vec3& l, vec3& intersect, float& t)
	{
		vec3 p0 = N * D;
		float denom	= dot(l, N);
		float p0l0	= dot(p0 - l0, N);
		

		if (fabsf(denom) < EPSILON)
		{
			// line paralell to the plane
			if (fabsf(p0l0) < EPSILON)
			{
				t = 0.0f;
				return true;
			}
			return false;
		}

		t = p0l0 / denom;
		intersect = l0 + l * t;

		return true;

	}
}
#endif // !JS3D_MATH_PLANE_H
