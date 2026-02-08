#version 150

uniform sampler2D tex;
uniform sampler2D bump;
uniform sampler2D decal;
uniform sampler2D shadowMap;
layout(std140) uniform PerFrame {
    mat4 projection;
    mat4 view;
    vec3 lightPosEye;
    mat4 shadowMatrix;
};
uniform bool has_bump_map;
uniform bool has_decal;
uniform bool debug_shadows;
uniform int pcf_mode;

in vec2 vTexCoords;
in vec2 vBumpCoords;
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
    // DEBUG: uncomment to verify geometry is being drawn
    // fragColor = vec4(1.0, 0.0, 1.0, 1.0); return;

    vec3 normalEye;
    if (has_bump_map) {
        vec3 normalTangent = normalize(texture(bump, vBumpCoords.st).rgb * 2.0 - 1.0);
        normalEye = normalize(vTBN * normalTangent);
    } else {
        normalEye = normalize(vTBN[2]);
    }
    float diffuseIntensity = max(0.0, dot(normalEye, -normalize(lightPosEye)));

    float shadow = calcShadow(vShadowCoord);

    if (debug_shadows && shadow < 1.0) {
        fragColor = vec4(0.0, 1.0, 0.0, 1.0);
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
