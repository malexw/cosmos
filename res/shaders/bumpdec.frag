#version 150

uniform sampler2D tex;
uniform sampler2D bump;
uniform sampler2D decal;
uniform sampler2DArray shadowMap;
layout(std140) uniform PerFrame {
    mat4 projection;
    mat4 view;
    vec3 lightPosEye;
    mat4 shadowMatrices[4];
    vec4 cascadeSplits;
    int cascadeCount;
    vec4 cascadeBiases;
};
uniform bool has_bump_map;
uniform bool has_decal;
uniform bool debug_shadows;
uniform int pcf_mode;

in vec2 vTexCoords;
in vec2 vBumpCoords;
in vec3 vMatDiffuse;
in mat3 vTBN;
in vec3 vWorldPos;
in float vViewDepth;

out vec4 fragColor;

float calcShadow() {
    int cascade = 0;
    for (int i = 0; i < cascadeCount; ++i)
        if (vViewDepth > cascadeSplits[i]) cascade = i + 1;
    if (cascade >= cascadeCount) return 1.0;

    vec4 sc = shadowMatrices[cascade] * vec4(vWorldPos, 1.0);
    if (sc.w <= 0.0) return 1.0;
    vec3 s = (sc / sc.w).xyz;
    s.z -= cascadeBiases[cascade];

    if (pcf_mode == 0) {
        return texture(shadowMap, vec3(s.xy, float(cascade))).r < s.z ? 0.5 : 1.0;
    }

    float shadow = 0.0;
    vec2 texelSize = vec2(1.0 / float(textureSize(shadowMap, 0).x));
    int range = pcf_mode == 1 ? 1 : 2;
    int count = 0;
    for (int x = -range; x <= range; x++) {
        for (int y = -range; y <= range; y++) {
            float d = texture(shadowMap, vec3(s.xy + vec2(x, y) * texelSize, float(cascade))).r;
            shadow += d < s.z ? 0.5 : 1.0;
            count++;
        }
    }
    return shadow / float(count);
}

void main(void)
{
    vec3 normalEye;
    if (has_bump_map) {
        vec3 normalTangent = normalize(texture(bump, vBumpCoords.st).rgb * 2.0 - 1.0);
        normalEye = normalize(vTBN * normalTangent);
    } else {
        normalEye = normalize(vTBN[2]);
    }
    float diffuseIntensity = max(0.0, dot(normalEye, -normalize(lightPosEye)));

    float shadow = calcShadow();

    if (debug_shadows && shadow < 1.0) {
        // Color-code by cascade
        int cascade = 0;
        for (int i = 0; i < cascadeCount; ++i)
            if (vViewDepth > cascadeSplits[i]) cascade = i + 1;
        if (cascade == 0)      fragColor = vec4(1.0, 0.0, 0.0, 1.0);
        else if (cascade == 1) fragColor = vec4(0.0, 1.0, 0.0, 1.0);
        else if (cascade == 2) fragColor = vec4(0.0, 0.0, 1.0, 1.0);
        else                   fragColor = vec4(1.0, 1.0, 0.0, 1.0);
        return;
    }

    vec3 color = (0.1 + diffuseIntensity * shadow) * vMatDiffuse;
    if (has_decal) {
        vec4 dec = texture(decal, vBumpCoords.st);
        if (dec.a < 0.1) {
            vec4 texel = texture(tex, vTexCoords.st);
            fragColor = vec4(texel.rgb * color, texel.a);
        } else {
            fragColor = vec4(dec.rgb * color, 1.0);
        }
    } else {
        vec4 texel = texture(tex, vTexCoords.st);
        fragColor = vec4(texel.rgb * color, texel.a);
    }
}
