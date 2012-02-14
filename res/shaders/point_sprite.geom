#version 150

uniform mat4 cosmos_RotationModelViewProjectionMatrix;

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
varying out vec2 tex_coords;

void main() {

  gl_Position = vec4(gl_PositionIn[0].xy + vec2(-0.5, 0.5), gl_PositionIn[0].zw);
  tex_coords = vec2(0.0, 0.0);
  EmitVertex();
  
  gl_Position = vec4(gl_PositionIn[0].xy + vec2(-0.5, -0.5), gl_PositionIn[0].zw);
  tex_coords = vec2(0.0, 1.0);
  EmitVertex();
  
  gl_Position = vec4(gl_PositionIn[0].xy + vec2(0.5, 0.5), gl_PositionIn[0].zw);
  tex_coords = vec2(1.0, 0.0);
  EmitVertex();
  
  gl_Position = vec4(gl_PositionIn[0].xy + vec2(0.5, -0.5), gl_PositionIn[0].zw);
  tex_coords = vec2(1.0, 1.0);
  EmitVertex();
  
  EndPrimitive();
}
