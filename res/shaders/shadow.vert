// Used for shadow lookup
varying vec4 ShadowCoord;
varying vec3 color;
varying vec2 vTexCoords;

void main()
{
  ShadowCoord= gl_TextureMatrix[3] * gl_Vertex;

  vec3 norm = gl_NormalMatrix * gl_Normal;
  // Using LightSource[0] as our directional "sun"
  vec3 lightDir = -normalize(gl_LightSource[0].position.xyz);
  float intensity = max(dot(norm, lightDir), 0.0);
  color = intensity * (gl_FrontMaterial.diffuse).rgb + (gl_FrontMaterial.ambient).rgb;
  vTexCoords = gl_MultiTexCoord0.st;

  gl_Position = ftransform();
}
