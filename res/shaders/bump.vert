
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 norm;

varying vec3 vNormal;
varying vec2 vTexCoords;
varying vec2 vBumpCoords;
varying vec3 vLightDir;
 
void main(void)
{
  vTexCoords = tex;
  vBumpCoords = tex;
  vNormal = gl_NormalMatrix * norm;

  gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0);
  
  vLightDir = gl_Position.xyz - gl_LightSource[0].position.xyz;
}
