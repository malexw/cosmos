#version 150

layout (std140) uniform matrices {
  mat4 c_ViewMatrix;
  mat4 c_ProjectionMatrix;
  mat4 c_ShadowMatrix;
  mat4 c_ModelMatrix;
  mat4 c_ModelViewMatrix;
  mat4 c_ModelViewProjectionMatrix;
};

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
