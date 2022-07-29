//#version 330 core
out vec4 vo_position;
out vec4 vo_qtangent;
out vec2 vo_st;
out vec4 vo_color;

void main() {
	gl_Position = va_position;

	vo_position = va_position;
	vo_qtangent = va_qtangent;
	vo_st = va_st * 64;
	vo_color = va_color;
} 
