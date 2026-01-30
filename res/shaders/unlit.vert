#version 150

in vec3 position;
in vec2 texCoord;

uniform mat4 mvp;

out vec2 vTexCoord;

void main()
{
  vTexCoord = texCoord;
  gl_Position = mvp * vec4(position, 1.0);
}
