#version 150

uniform sampler2D tex;

in vec2 vTexCoord;

out vec4 fragColor;

void main()
{
  fragColor = texture(tex, vTexCoord);
}
