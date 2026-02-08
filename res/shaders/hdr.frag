#version 150

const float PI = 3.14159265359;

uniform sampler2D tex;
uniform float exposure;

in vec3 vPosition;

out vec4 fragColor;

void main()
{
  vec3 dir = normalize(vPosition);
  vec2 uv = vec2(0.5 + atan(dir.z, dir.x) / (2.0 * PI),
                  0.5 - asin(dir.y) / PI);

  vec4 texel = texture(tex, uv);

  // SOURCE
  // High Dynamic Range Rendering in OpenGL
  // Fabien Houlmann, Stéphane Metz
  // http://transporter-game.googlecode.com/files/HDRRenderingInOpenGL.pdf
  float Y = dot(vec4(0.30, 0.59, 0.11, 0.0), texel);
  Y = Y * exposure;
  Y = Y / (Y + 1.0);
  texel.rgb = texel.rgb * Y;

  fragColor = vec4(texel.rgb, 1.0);
}
