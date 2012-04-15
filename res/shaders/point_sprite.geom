#version 150

#include cosmos.matrices

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
varying out vec2 tex_coords;

void main() {

  vec4 offset_tl = c_ProjectionMatrix * vec4(-0.5, 0.5, 0.0, 1.0);

  gl_Position = gl_PositionIn[0] + offset_tl;
  tex_coords = vec2(0.0, 0.0);
  EmitVertex();

  vec4 offset_bl = c_ProjectionMatrix * vec4(-0.5, -0.5, 0.0, 1.0);

  gl_Position = gl_PositionIn[0] + offset_bl;
  tex_coords = vec2(0.0, 1.0);
  EmitVertex();

  gl_Position = gl_PositionIn[0] + (c_ProjectionMatrix * vec4(0.5, 0.5, 0.0, 1.0));
  //gl_Position = vec4(gl_PositionIn[0].xy + vec2(0.5, 0.5), gl_PositionIn[0].zw);
  tex_coords = vec2(1.0, 0.0);
  EmitVertex();

  gl_Position = gl_PositionIn[0] + (c_ProjectionMatrix * vec4(0.5, -0.5, 0.0, 1.0));
  //gl_Position = vec4(gl_PositionIn[0].xy + vec2(0.5, -0.5), gl_PositionIn[0].zw);
  tex_coords = vec2(1.0, 1.0);
  EmitVertex();

  EndPrimitive();
}
