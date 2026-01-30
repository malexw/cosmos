#version 150

uniform sampler2D tex;
uniform sampler2D bump;
uniform sampler2D decal;
uniform vec3 lightPosEye;

in vec3 vVaryingNormal;
in vec2 vTexCoords;
in vec2 vBumpCoords;
in vec3 vMatDiffuse;

out vec4 fragColor;

void main(void)
{
	vec3 normalAdjusted = vVaryingNormal + normalize((texture(bump, vBumpCoords.st).rgb - 0.5)*2.0);
  float diffuseIntensity = max(0.0, dot(normalize(normalAdjusted), -normalize(lightPosEye)));

  vec3 color = (0.1 + diffuseIntensity) * vMatDiffuse;
  vec4 dec = texture(decal, vBumpCoords.st);
  if (dec.a < 0.1) {
    vec4 texel = texture(tex, vTexCoords.st);
    fragColor = vec4(texel.rgb * color, texel.a);
  } else {
    fragColor = vec4(dec.rgb * color, 1.0);
  }
}
