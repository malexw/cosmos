#version 150

layout (location = 0) in vec3 pos;

layout (std140) uniform matrices {
  mat4 c_ViewMatrix;
  mat4 c_ProjectionMatrix;
  mat4 c_ShadowMatrix;
  mat4 c_ModelMatrix;
  mat4 c_ModelViewMatrix;
  mat4 c_ModelViewProjectionMatrix;
};

void main(void)
{
  gl_Position = c_ModelViewProjectionMatrix * vec4(pos, 1.0);
}
