#version 150

in vec3 position;
in vec2 texCoord;

uniform mat4 mvp;

out vec2 vTexCoords;

void main()
{
  vTexCoords = texCoord;

  gl_Position = mvp * vec4(position, 1.0);
}
