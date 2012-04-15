#version 150

#include cosmos.matrices

uniform sampler2D tex;

varying vec3 vNormal;
varying vec2 vTexCoords;
varying vec3 vLightDir;

void main(void)
{
  //float diffuseIntensity = clamp(dot(normalize(vNormal), normalize(vLightDir)), 0.0, 1.0);

  //vec3 color = diffuseIntensity * (gl_FrontMaterial.diffuse).rgb + (gl_FrontMaterial.ambient).rgb;
  vec4 texel = texture2D(tex, vTexCoords.st);
  //gl_FragColor = vec4(texel.rgb * color, texel.a * gl_FrontMaterial.diffuse.a);
  gl_FragColor = texel;
}
