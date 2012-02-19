#version 150

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;

layout (std140) uniform matrices {
  mat4 c_ViewMatrix;
  mat4 c_ProjectionMatrix;
  mat4 c_ShadowMatrix;
  mat4 c_ModelMatrix;
  mat4 c_ModelViewMatrix;
  mat4 c_ModelViewProjectionMatrix;
};

uniform vec3 light_pos;

varying vec3 vNormal;
varying vec2 vTexCoords;
varying vec3 vLightDir;

void main(void)
{
  vTexCoords = tex;
  vNormal = normalize(c_ModelViewMatrix * vec4(norm, 0.0)).xyz;

  gl_Position = c_ModelViewProjectionMatrix * vec4(pos, 1.0);

  vLightDir = (c_ViewMatrix * vec4(light_pos, 1.0)).xyz - (c_ModelViewMatrix * vec4(pos, 1.0)).xyz;
}
