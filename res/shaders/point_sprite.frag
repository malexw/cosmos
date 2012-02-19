#version 150

layout (std140) uniform matrices {
  mat4 c_ViewMatrix;
  mat4 c_ProjectionMatrix;
  mat4 c_ShadowMatrix;
  mat4 c_ModelMatrix;
  mat4 c_ModelViewMatrix;
  mat4 c_ModelViewProjectionMatrix;
};

uniform sampler2D tex;

varying vec2 tex_coords;

void main(void)
{
  vec4 texel = texture2D(tex, tex_coords);
  gl_FragColor = vec4(texel);
}
