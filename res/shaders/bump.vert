//in vec4 vVertex;
//in vec3 vNormal;
//in vec4 vTexture;
 
//uniform mat4 mvpMatrix;
//uniform mat4 mvMatrix;
//uniform mat3 normalMatrix;
//uniform vec3 vLightPosition;
 
// Outgoing normal and light direction to fragment shader
//varying vec4 diffuse, ambient;
varying vec3 vVaryingNormal;
varying vec3 vVaryingLightDir;
varying vec2 vTexCoords;
varying vec2 vBumpCoords;
 
void main(void)
{
 
    // Get surface normal in eye coordinates and pass them through to the fragment shader
    vVaryingNormal = gl_NormalMatrix * gl_Normal;
    // Get vector to light source
    vVaryingLightDir = normalize(gl_Vertex.xyz - vec3(gl_LightSource[0].position));
 
    // Pass the texture coordinates through the vertex shader so they get smoothly interpolated
    vTexCoords = gl_MultiTexCoord0.st;
    vBumpCoords = gl_MultiTexCoord1.st;
    
    //diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
    //ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
    //ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;
 
    // Transform the geometry through the modelview-projection matrix
    //gl_Position = mvpMatrix * vVertex;
    gl_Position = ftransform();
}
