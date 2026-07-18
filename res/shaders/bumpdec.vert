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
    mat4 shadowMatrices[4];
    vec4 cascadeSplits;
    int cascadeCount;
    int lightCount;
    int pointShadowIndex;
    vec4 cascadeBiases;
    vec4 lightPosDir[8];
    vec4 lightColor[8];
    vec4 pointShadowPos;
    vec4 pointShadowParams;
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
out vec3 vEyePos;

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
    vec4 eyePos = view * worldPos;
    vWorldPos = worldPos.xyz;
    vEyePos = eyePos.xyz;

    gl_Position = projection * eyePos;
}
