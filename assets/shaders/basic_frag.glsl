#version 330 core

out vec4 fragColor;

in vec4 vo_normal;
in vec4 vo_tangent;
in vec2 vo_st;
in vec4 vo_position;

vec3 linear2SRGB(vec3 p) {
	float k = 2.2;
	return vec3(
		pow(p.r, k),
		pow(p.g, k),
		pow(p.b, k)
	);
}

void main() {
	//linear2SRGB( 2*vo_normal-1 )
	fragColor = 2*vo_normal-1 ;
}