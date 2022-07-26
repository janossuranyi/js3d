#version 330 core

layout(location = 0) in vec4 va_position;
layout(location = 1) in vec4 va_normal;
layout(location = 2) in vec4 va_tangent;
layout(location = 3) in vec2 va_st;

out vec4 vo_normal;
out vec4 vo_tangent;
out vec2 vo_st;
out vec4 vo_position;

void main() {
	gl_Position = va_position;

	vo_position = va_position;
	vo_normal = va_normal;	
	vo_tangent = va_tangent;
	vo_st = va_st;
} 
