#version 330

uniform sampler2D tex;

in vec2 vTexCoord;
in vec4 vColor;

out vec4 fragColor;

void main()
{
    fragColor = texture(tex, vTexCoord) * vColor;
}
