#version 150

uniform sampler2D tex;
uniform float exposure;

in vec2 vTexCoords;

out vec4 fragColor;

void main()
{
  vec4 texel = texture(tex, vTexCoords.st);

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
