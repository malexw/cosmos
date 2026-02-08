#version 150

in vec3 position;
in vec2 texCoord;
in vec3 normal;
in vec3 color;
in vec3 tangent;
in vec3 bitangent;

// Per-instance mat4 (4 vec4s at locations 6-9)
in vec4 instanceModelCol0;
in vec4 instanceModelCol1;
in vec4 instanceModelCol2;
in vec4 instanceModelCol3;

layout(std140) uniform PerFrame {
    mat4 projection;
    mat4 view;
    vec3 lightPosEye;
    mat4 shadowMatrix;
};

out vec2 vTexCoords;
out vec2 vBumpCoords;
out vec3 vMatDiffuse;
out mat3 vTBN;
out vec4 vShadowCoord;

void main()
{
    mat4 model = mat4(instanceModelCol0, instanceModelCol1,
                      instanceModelCol2, instanceModelCol3);
    mat3 normalMatrix = mat3(view) * transpose(inverse(mat3(model)));

    vec3 T = normalize(normalMatrix * tangent);
    vec3 B = normalize(normalMatrix * bitangent);
    vec3 N = normalize(normalMatrix * normal);
    vTBN = mat3(T, B, N);

    vTexCoords = texCoord;
    vBumpCoords = texCoord;
    vMatDiffuse = color;
    vShadowCoord = shadowMatrix * model * vec4(position, 1.0);

    gl_Position = projection * view * model * vec4(position, 1.0);
}
