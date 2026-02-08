#version 150

in vec3 position;
in vec2 texCoord;
in vec3 normal;
in vec3 color;
in vec3 tangent;
in vec3 bitangent;

uniform mat4 mvp;
uniform mat3 normalMatrix;

out vec2 vTexCoords;
out vec2 vBumpCoords;
out vec3 vMatDiffuse;
out mat3 vTBN;

void main(void)
{
    vec3 T = normalize(normalMatrix * tangent);
    vec3 B = normalize(normalMatrix * bitangent);
    vec3 N = normalize(normalMatrix * normal);
    vTBN = mat3(T, B, N);

    vTexCoords = texCoord;
    vBumpCoords = texCoord;
    vMatDiffuse = color;

    gl_Position = mvp * vec4(position, 1.0);
}
