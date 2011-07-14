uniform sampler2D shadowMap;
uniform sampler2D tex;

varying vec4 ShadowCoord;
varying vec3 color;
varying vec2 vTexCoords;

void main()
{	
  vec4 shadowCoordinateWdivide = ShadowCoord / ShadowCoord.w ;
  
  // Used to lower moiré pattern and self-shadowing
  shadowCoordinateWdivide.z += 0.001;
  //shadowCoordinateWdivide.z += 0.0005;
  
  float distanceFromLight = texture2D(shadowMap,shadowCoordinateWdivide.st).z;
  
  float shadow = 1.0;
  if (ShadowCoord.w > 0.0) {
    shadow = distanceFromLight < shadowCoordinateWdivide.z ? 0.5 : 1.0 ;
  }
  
  vec4 texel = texture2D(tex, vTexCoords.st); //vec4(1.0, 1.0, 1.0, 1.0);
  gl_FragColor = shadow * vec4(texel.rgb * color, texel.a * gl_FrontMaterial.diffuse.a);
}
