#version 150

uniform sampler2D tex;

layout (std140) uniform matrices {
  mat4 c_ViewMatrix;
  mat4 c_ProjectionMatrix;
  mat4 c_ShadowMatrix;
  mat4 c_ModelMatrix;
  mat4 c_ModelViewMatrix;
  mat4 c_ModelViewProjectionMatrix;
};

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
