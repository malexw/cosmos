#version 150

uniform sampler2DArray shadowMap;
uniform samplerCube pointShadowMap;
uniform bool debug_shadows;
uniform int pcf_mode;

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

in vec3 vMatDiffuse;
in mat3 vTBN;
in vec3 vWorldPos;
in vec3 vEyePos;

out vec4 fragColor;

float calcShadow() {
    int cascade = 0;
    for (int i = 0; i < cascadeCount; ++i)
        if (-vEyePos.z > cascadeSplits[i]) cascade = i + 1;
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

// Omnidirectional shadow lookup for the light at pointShadowIndex.
// pointShadowPos.xyz = light world position; pointShadowParams = (near, far, bias)
float calcPointShadow() {
    vec3 fragToLight = vWorldPos - pointShadowPos.xyz;
    vec3 a = abs(fragToLight);
    // Distance along the sampled face's major axis — matches the face's depth axis
    float dist = max(a.x, max(a.y, a.z));

    float near = pointShadowParams.x;
    float far = pointShadowParams.y;
    float stored = texture(pointShadowMap, fragToLight).r;
    // Linearize the stored perspective depth back to a distance
    float storedDist = (2.0 * near * far) / (far + near - (stored * 2.0 - 1.0) * (far - near));

    // Full occlusion: unlike the sun's soft 0.5, no fill light leaks through
    return storedDist < dist - pointShadowParams.z ? 0.0 : 1.0;
}

// lightPosDir[i].xyz: eye-space position (point) or travel direction (directional)
// lightPosDir[i].w: 0 = directional, 1 = point
// lightColor[i].rgb: color * intensity; lightColor[i].w: attenuation radius (point)
vec3 calcLighting(vec3 normalEye, float sunShadow) {
    vec3 lighting = vec3(0.0);
    for (int i = 0; i < lightCount; ++i) {
        vec3 L;
        float att = 1.0;
        if (lightPosDir[i].w == 0.0) {
            L = -normalize(lightPosDir[i].xyz);
            // Only the primary directional light casts (cascaded) shadows
            if (i == 0) att = sunShadow;
        } else {
            vec3 toLight = lightPosDir[i].xyz - vEyePos;
            float dist = length(toLight);
            L = toLight / dist;
            float falloff = clamp(1.0 - dist / lightColor[i].w, 0.0, 1.0);
            att = falloff * falloff;
            if (i == pointShadowIndex) att *= calcPointShadow();
        }
        lighting += lightColor[i].rgb * (max(0.0, dot(normalEye, L)) * att);
    }
    return lighting;
}

void main(void)
{
    vec3 normalEye = normalize(vTBN[2]);

    float shadow = calcShadow();

    if (debug_shadows && shadow < 1.0) {
        int cascade = 0;
        for (int i = 0; i < cascadeCount; ++i)
            if (-vEyePos.z > cascadeSplits[i]) cascade = i + 1;
        if (cascade == 0)      fragColor = vec4(1.0, 0.0, 0.0, 1.0);
        else if (cascade == 1) fragColor = vec4(0.0, 1.0, 0.0, 1.0);
        else if (cascade == 2) fragColor = vec4(0.0, 0.0, 1.0, 1.0);
        else                   fragColor = vec4(1.0, 1.0, 0.0, 1.0);
        return;
    }

    vec3 color = (vec3(0.1) + calcLighting(normalEye, shadow)) * vMatDiffuse;
    fragColor = vec4(color, 1.0);
}
