#version 150

in vec3 position;
in vec2 texCoord;
in vec3 normal;
in vec3 color;
in vec3 tangent;
in vec3 bitangent;

layout(std140) uniform PerFrame {
    mat4 projection;
    mat4 view;
    vec3 lightPosEye;
    mat4 shadowMatrices[4];
    vec4 cascadeSplits;
    int cascadeCount;
    vec4 cascadeBiases;
};

layout(std140) uniform PerDraw {
    mat4 model;
    mat3 normalMatrix;
};

out vec2 vTexCoords;
out vec2 vBumpCoords;
out vec3 vMatDiffuse;
out mat3 vTBN;
out vec3 vWorldPos;
out float vViewDepth;

void main(void)
{
    vec3 T = normalize(normalMatrix * tangent);
    vec3 B = normalize(normalMatrix * bitangent);
    vec3 N = normalize(normalMatrix * normal);
    vTBN = mat3(T, B, N);

    vTexCoords = texCoord;
    vBumpCoords = texCoord;
    vMatDiffuse = color;

    vec4 worldPos = model * vec4(position, 1.0);
    vWorldPos = worldPos.xyz;
    vViewDepth = -(view * worldPos).z;

    gl_Position = projection * view * worldPos;
}
