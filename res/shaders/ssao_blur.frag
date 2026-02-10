#version 150

uniform sampler2D ssaoTex;
uniform sampler2D depthTex;

in vec2 vTexCoord;
out vec4 fragColor;

const float spatialSigma = 2.0;
const float depthSigma = 0.001;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoTex, 0));
    float centerDepth = texture(depthTex, vTexCoord).r;
    float centerAO = texture(ssaoTex, vTexCoord).r;

    float result = 0.0;
    float totalWeight = 0.0;

    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            vec2 offset = vec2(float(x), float(y));
            vec2 sampleUV = vTexCoord + offset * texelSize;

            float sampleAO = texture(ssaoTex, sampleUV).r;
            float sampleDepth = texture(depthTex, sampleUV).r;

            // Spatial Gaussian weight
            float dist2 = dot(offset, offset);
            float spatialW = exp(-dist2 / (2.0 * spatialSigma * spatialSigma));

            // Depth similarity weight (soft falloff)
            float depthDiff = centerDepth - sampleDepth;
            float depthW = exp(-(depthDiff * depthDiff) / (2.0 * depthSigma * depthSigma));

            float w = spatialW * depthW;
            result += sampleAO * w;
            totalWeight += w;
        }
    }

    fragColor = vec4(result / totalWeight);
}
