#version 150

in vec3 position;
in vec2 texCoord;
in vec3 normal;
in vec3 color;

uniform mat4 mvp;
uniform mat3 normalMatrix;
uniform vec3 lightPosEye;
uniform vec4 lightDiffuse;
uniform vec4 lightAmbient;
uniform vec4 lightSpecular;
uniform vec4 ambientGlobal;

out vec4 diffuse, ambient;
out vec3 vNormal, lightDir, halfVec;

void main()
{
  vNormal = normalize(normalMatrix * normal);
  lightDir = normalize(lightPosEye);
  halfVec = normalize(normalize(lightPosEye) + vec3(0.0, 0.0, 1.0));

  diffuse = vec4(color, 1.0) * lightDiffuse;
  ambient = vec4(color, 1.0) * lightAmbient;
  ambient += ambientGlobal * vec4(color, 1.0);

  gl_Position = mvp * vec4(position, 1.0);
}
