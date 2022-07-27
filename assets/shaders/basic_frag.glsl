#version 330 core

out vec4 fragColor;

in vec4 vo_position;
in vec4 vo_qtangent;
in vec2 vo_st;
in vec4 vo_color;

const float kGamma = 2.2;

vec4 rgb_to_srgb(vec4 p) {
	return vec4(
		pow(p.r, kGamma),
		pow(p.g, kGamma),
		pow(p.b, kGamma),
		p.a
	);
}

void main() {
	
	vec4 color = vo_color;
	fragColor = rgb_to_srgb(color);
}