#version 150

in vec3 position;
in vec2 texCoord;
in vec3 normal;
in vec3 color;

uniform mat4 mvp;
uniform mat3 normalMatrix;
uniform mat4 shadowMatrix;
uniform vec3 lightPosEye;

// Used for shadow lookup
out vec4 ShadowCoord;
out vec3 vColor;
out vec2 vTexCoords;

void main()
{
  ShadowCoord = shadowMatrix * vec4(position, 1.0);

  vec3 norm = normalMatrix * normal;
  // Using light direction as our directional "sun"
  vec3 lightDir = -normalize(lightPosEye);
  float intensity = max(dot(norm, lightDir), 0.0);
  vColor = (0.1 + intensity) * color;
  vTexCoords = texCoord;

  gl_Position = mvp * vec4(position, 1.0);
}
