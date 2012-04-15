#version 150

#include cosmos.attrib_array
#include cosmos.matrices

void main(void)
{
  gl_Position = c_ModelViewProjectionMatrix * vec4(pos, 1.0);
}
