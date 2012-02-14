#version 150

layout (location = 0) in vec3 pos;

void main(void)
{
  gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0);
}
