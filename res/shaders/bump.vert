#version 150

in vec3 position;
in vec2 texCoord;
in vec3 normal;
in vec3 color;

uniform mat4 mvp;
uniform mat3 normalMatrix;

out vec3 vVaryingNormal;
out vec2 vTexCoords;
out vec2 vBumpCoords;
out vec3 vMatDiffuse;

void main(void)
{
    vVaryingNormal = normalMatrix * normal;
    vTexCoords = texCoord;
    vBumpCoords = texCoord;
    vMatDiffuse = color;

    gl_Position = mvp * vec4(position, 1.0);
}
