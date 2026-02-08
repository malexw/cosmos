#version 150

uniform sampler2D tex;
uniform bool hdr_output;
uniform float sdr_white;
uniform float hdr_headroom;
uniform float exposure;

in vec2 vTexCoord;
out vec4 fragColor;

void main()
{
    vec3 color = texture(tex, vTexCoord).rgb;
    color *= exposure;

    if (hdr_output) {
        // HDR path: output linear scRGB scaled to SDR white level
        // Compositor (Gamescope) handles PQ conversion
        fragColor = vec4(color * sdr_white, 1.0);
    } else {
        // SDR path: Reinhard tone mapping
        float Y = dot(vec3(0.30, 0.59, 0.11), color);
        float Yt = Y / (Y + 1.0);
        vec3 mapped = color * (Yt / max(Y, 0.0001));
        fragColor = vec4(mapped, 1.0);
    }
}
