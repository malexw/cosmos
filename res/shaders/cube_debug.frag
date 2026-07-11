#version 150

uniform samplerCube cubeMap;
uniform int face;
uniform float near_plane;
uniform float far_plane;

in vec2 vTexCoord;

out vec4 fragColor;

// Reconstructs the cube map sample direction for a given face from quad UVs,
// matching OpenGL's cube map face coordinate conventions.
vec3 faceDir(vec2 uv) {
    if (face == 0) return vec3( 1.0, -uv.y, -uv.x);  // +X
    if (face == 1) return vec3(-1.0, -uv.y,  uv.x);  // -X
    if (face == 2) return vec3( uv.x,  1.0,  uv.y);  // +Y
    if (face == 3) return vec3( uv.x, -1.0, -uv.y);  // -Y
    if (face == 4) return vec3( uv.x, -uv.y,  1.0);  // +Z
    return vec3(-uv.x, -uv.y, -1.0);                 // -Z
}

void main()
{
    // Quad texcoords increase upward on screen, cube face t increases downward
    vec2 uv = vec2(vTexCoord.x, 1.0 - vTexCoord.y) * 2.0 - 1.0;
    float d = texture(cubeMap, faceDir(uv)).r;

    // Linearize (perspective depth crowds near 1.0); near = white, far = black
    float ndc = d * 2.0 - 1.0;
    float linear = (2.0 * near_plane * far_plane) /
                   (far_plane + near_plane - ndc * (far_plane - near_plane));
    fragColor = vec4(vec3(1.0 - linear / far_plane), 1.0);
}
