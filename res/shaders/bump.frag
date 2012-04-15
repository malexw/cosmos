#version 150

#include cosmos.matrices

uniform sampler2D tex;
uniform sampler2D bump;
uniform sampler2D decal;

uniform vec3 light_pos;

varying vec3 vNormal;
varying vec2 vTexCoords;
varying vec2 vBumpCoords;
varying vec3 vLightDir;

void main(void)
{
  vec3 normalAdjusted = vNormal + normalize((texture2D(bump, vBumpCoords).rgb - 0.5)*1.0);
  float diffuseIntensity = clamp(dot(normalize(normalAdjusted), normalize(vLightDir)), 0.0, 1.0);

  vec3 color = diffuseIntensity * (gl_FrontMaterial.diffuse).rgb + (gl_FrontMaterial.ambient).rgb;
  vec4 texel = texture2D(tex, vTexCoords.st);
  gl_FragColor = vec4(texel.rgb * color, texel.a * gl_FrontMaterial.diffuse.a);
  //gl_FragColor = vec4(diffuseIntensity, diffuseIntensity, diffuseIntensity, 1.0);
}
