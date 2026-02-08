#version 150

uniform sampler2D shadowMap;
uniform bool debug_shadows;
uniform int pcf_mode;

layout(std140) uniform PerFrame {
    mat4 projection;
    mat4 view;
    vec3 lightPosEye;
    mat4 shadowMatrix;
};

in vec3 vMatDiffuse;
in mat3 vTBN;
in vec4 vShadowCoord;

out vec4 fragColor;

float calcShadow(vec4 sc) {
    if (sc.w <= 0.0) return 1.0;
    vec4 s = sc / sc.w;
    s.z -= 0.005;

    if (pcf_mode == 0) {
        return texture(shadowMap, s.st).r < s.z ? 0.5 : 1.0;
    }

    float shadow = 0.0;
    vec2 texelSize = vec2(1.0 / 2048.0);
    int range = pcf_mode == 1 ? 1 : 2;
    int count = 0;
    for (int x = -range; x <= range; x++) {
        for (int y = -range; y <= range; y++) {
            float d = texture(shadowMap, s.st + vec2(x, y) * texelSize).r;
            shadow += d < s.z ? 0.5 : 1.0;
            count++;
        }
    }
    return shadow / float(count);
}

void main(void)
{
    vec3 normalEye = normalize(vTBN[2]);
    float diffuseIntensity = max(0.0, dot(normalEye, -normalize(lightPosEye)));

    float shadow = calcShadow(vShadowCoord);

    if (debug_shadows && shadow < 1.0) {
        fragColor = vec4(0.0, 1.0, 0.0, 1.0);
        return;
    }

    vec3 color = (0.1 + diffuseIntensity * shadow) * vMatDiffuse;
    fragColor = vec4(color, 1.0);
}
