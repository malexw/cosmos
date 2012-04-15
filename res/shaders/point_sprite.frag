#version 150

#include cosmos.matrices

uniform sampler2D tex;

varying vec2 tex_coords;

void main(void)
{
  vec4 texel = texture2D(tex, tex_coords);
  gl_FragColor = vec4(texel);
}
