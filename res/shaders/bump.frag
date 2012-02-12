uniform sampler2D tex;
uniform sampler2D bump;
uniform sampler2D decal;

//varying vec4 diffuse, ambient;
varying vec3 vNormal;
//varying vec3 vVaryingLightDir;
varying vec2 vTexCoords;
varying vec2 vBumpCoords;
varying vec3 vLightDir;

void main(void)
{
  vec3 normalAdjusted = vNormal + normalize((texture2D(bump, vBumpCoords).rgb - 0.5)*2.0);
  float diffuseIntensity = max(0.0, dot(normalize(normalAdjusted), normalize(vLightDir)));

  vec3 color = diffuseIntensity * (gl_FrontMaterial.diffuse).rgb + (gl_FrontMaterial.ambient).rgb;
  vec4 texel = texture2D(tex, vTexCoords.st);
  gl_FragColor = vec4(texel.rgb * color, texel.a * gl_FrontMaterial.diffuse.a);
}
