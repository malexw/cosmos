uniform sampler2D tex;

varying vec2 vTexCoords;

void main()
{	
  //vec3 color = (gl_FrontMaterial.diffuse).rgb + (gl_FrontMaterial.ambient).rgb;

  vec4 texel = texture2D(tex, vTexCoords.st);
  
  float Y = dot(vec4(0.30, 0.59, 0.11, 0.0), texel);
  float YD = 2.2 * (2.2/10.0 + 1.0) / (2.2 + 1.0);
  texel *= YD;
  gl_FragColor = texel;

  
  //gl_FragColor = vec4(texel.rgb * exp2(texel.a * 5.0), 1.0);
  
  //ivec4 rgbe = textureCube( sampler, coords ) * 255. + 0.5;
  //float e = texel.a - ( 128 + 8 );
  //gl_FragColor = vec4(texel.rgb * exp2(e), 1.0);
  
  //if (texel.r == 0.0) {
    //gl_FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
  //} else {
    //gl_FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    //gl_FragColor = vec4(1.0 - exp2(-texel.rgb) /** color*/, 1.0/*texel.a * gl_FrontMaterial.diffuse.a*/);
  //}
}
