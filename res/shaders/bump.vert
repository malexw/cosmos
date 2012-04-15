//varying vec4 diffuse, ambient;
varying vec3 vVaryingNormal;
//varying vec3 vVaryingLightDir;
varying vec2 vTexCoords;
varying vec2 vBumpCoords;
 
void main(void)
{

    vVaryingNormal = gl_NormalMatrix * gl_Normal;
    vTexCoords = gl_MultiTexCoord0.st;
    vBumpCoords = gl_MultiTexCoord0.st;
    
    //diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
    //ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
    //ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;

    gl_Position = ftransform();
}
