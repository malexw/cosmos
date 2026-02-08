#version 150

in vec3 position;

// Per-instance mat4 (4 vec4s at locations 6-9)
in vec4 instanceModelCol0;
in vec4 instanceModelCol1;
in vec4 instanceModelCol2;
in vec4 instanceModelCol3;

uniform mat4 lightViewProj;

void main()
{
    mat4 model = mat4(instanceModelCol0, instanceModelCol1,
                      instanceModelCol2, instanceModelCol3);
    gl_Position = lightViewProj * model * vec4(position, 1.0);
}
