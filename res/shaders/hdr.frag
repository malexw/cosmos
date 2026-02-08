#version 150

const float PI = 3.14159265359;

uniform sampler2D tex;

in vec3 vPosition;

out vec4 fragColor;

void main()
{
  vec3 dir = normalize(vPosition);
  vec2 uv = vec2(0.5 + atan(dir.z, dir.x) / (2.0 * PI),
                  0.5 - asin(dir.y) / PI);

  vec4 texel = texture(tex, uv);
  fragColor = vec4(texel.rgb, 1.0);
}
