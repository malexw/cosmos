#version 150

uniform sampler2D depthTex;
uniform sampler2D noiseTex;

uniform vec3 samples[64];
uniform mat4 projection;
uniform vec2 screenSize;
uniform float radius;
uniform float bias;
uniform float power;

in vec2 vTexCoord;
out vec4 fragColor;

vec3 viewPosFromDepth(vec2 uv, float depth) {
    // Convert depth from [0,1] to NDC [-1,1]
    float z_ndc = depth * 2.0 - 1.0;
    // Reconstruct clip-space position
    vec4 clipPos = vec4(uv * 2.0 - 1.0, z_ndc, 1.0);
    // Inverse projection to view space
    vec4 viewPos = inverse(projection) * clipPos;
    return viewPos.xyz / viewPos.w;
}

void main()
{
    float depth = texture(depthTex, vTexCoord).r;

    // Skip fragments at far plane (sky)
    if (depth >= 1.0) {
        fragColor = vec4(1.0);
        return;
    }

    vec3 fragPos = viewPosFromDepth(vTexCoord, depth);

    // Reconstruct view-space normal from depth derivatives
    vec3 dPdx = dFdx(fragPos);
    vec3 dPdy = dFdy(fragPos);
    vec3 normal = normalize(cross(dPdx, dPdy));

    // Noise for random TBN rotation (tiled every 4 pixels)
    vec2 noiseUV = vTexCoord * (screenSize / 4.0);
    vec3 randomVec = texture(noiseTex, noiseUV).xyz;

    // Build TBN matrix
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // Sample and accumulate occlusion
    float occlusion = 0.0;
    for (int i = 0; i < 64; ++i) {
        // Transform sample from tangent space to view space
        vec3 samplePos = fragPos + TBN * samples[i] * radius;

        // Project sample to screen space
        vec4 offset = projection * vec4(samplePos, 1.0);
        offset.xy /= offset.w;
        offset.xy = offset.xy * 0.5 + 0.5;

        // Sample depth at projected position
        float sampleDepth = texture(depthTex, offset.xy).r;
        vec3 sampleViewPos = viewPosFromDepth(offset.xy, sampleDepth);

        // Range check and occlusion test
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleViewPos.z));
        occlusion += (sampleViewPos.z >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / 64.0);
    occlusion = pow(occlusion, power);
    fragColor = vec4(occlusion);
}
