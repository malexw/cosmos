#version 150

#include cosmos.attrib_array
#include cosmos.matrices

uniform vec3 light_pos;

varying vec3 vNormal;
varying vec2 vTexCoords;
varying vec2 vBumpCoords;
varying vec3 vLightDir;

void main(void)
{
  vTexCoords = tex;
  vBumpCoords = tex;
  vNormal = normalize(c_ModelViewMatrix * vec4(norm, 0.0)).xyz;

  gl_Position = c_ModelViewProjectionMatrix * vec4(pos, 1.0);

  vLightDir = (c_ViewMatrix * vec4(light_pos, 1.0)).xyz - (c_ModelViewMatrix * vec4(pos, 1.0)).xyz;
}
