#version 330

// Per-vertex (from quad mesh VBO)
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

// Per-instance (from instance VBO, divisor = 1)
layout(location = 6) in vec3 instancePosition;
layout(location = 7) in vec4 instanceColor;
layout(location = 8) in float instanceScale;

uniform mat4 viewProj;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

out vec2 vTexCoord;
out vec4 vColor;

void main()
{
    vTexCoord = texCoord;
    vColor = instanceColor;

    // Billboard: expand quad vertex in camera-aligned space
    vec3 worldPos = instancePosition
                  + cameraRight * position.x * instanceScale
                  + cameraUp    * position.y * instanceScale;

    gl_Position = viewProj * vec4(worldPos, 1.0);
}
