#version 150

uniform sampler2D shadowMap;
uniform sampler2D tex;

in vec4 ShadowCoord;
in vec3 vColor;
in vec2 vTexCoords;

out vec4 fragColor;

void main()
{
  vec4 shadowCoordinateWdivide = ShadowCoord / ShadowCoord.w ;

  // Used to lower moiré pattern and self-shadowing
  shadowCoordinateWdivide.z += 0.001;
  //shadowCoordinateWdivide.z += 0.0005;

  float distanceFromLight = texture(shadowMap,shadowCoordinateWdivide.st).r;

  float shadow = 1.0;
  if (ShadowCoord.w > 0.0) {
    shadow = distanceFromLight < shadowCoordinateWdivide.z ? 0.5 : 1.0 ;
  }

  vec4 texel = texture(tex, vTexCoords.st);
  fragColor = vec4(texel.rgb * vColor * shadow, texel.a);
}
