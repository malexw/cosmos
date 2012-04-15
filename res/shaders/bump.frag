uniform sampler2D tex;
uniform sampler2D bump;
uniform sampler2D decal;

//varying vec4 diffuse, ambient;
varying vec3 vVaryingNormal;
//varying vec3 vVaryingLightDir;
varying vec2 vTexCoords;
varying vec2 vBumpCoords;
 
void main(void)
{ 
	vec3 normalAdjusted = vVaryingNormal + normalize((texture2D(bump, vBumpCoords.st).rgb - 0.5)*2.0);
  float diffuseIntensity = max(0.0, dot(normalize(normalAdjusted), -normalize(gl_LightSource[0].position.xyz)));
 
  vec3 color = diffuseIntensity * (gl_FrontMaterial.diffuse).rgb + (gl_FrontMaterial.ambient).rgb;
  vec4 texel = texture2D(tex, vTexCoords.st);
  gl_FragColor = vec4(texel.rgb * color, texel.a * gl_FrontMaterial.diffuse.a);
}
