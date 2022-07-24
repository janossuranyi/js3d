#version 410 core

void main()
{
  gl_Position = rpMtxModelViewProj * vec4(va_Position, 1.0);
}
