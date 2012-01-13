uniform sampler2D tex;
uniform float exposure;

varying vec2 vTexCoords;

void main()
{	
  // May want this if I decide to allow color material in HDR images
  //vec3 color = (gl_FrontMaterial.diffuse).rgb + (gl_FrontMaterial.ambient).rgb;

  vec4 texel = texture2D(tex, vTexCoords.st);
  
  // SOURCE
  // High Dynamic Range Rendering in OpenGL
  // Fabien Houlmann, Stéphane Metz
  // http://transporter-game.googlecode.com/files/HDRRenderingInOpenGL.pdf
  float Y = dot(vec4(0.30, 0.59, 0.11, 0.0), texel);
  //float YD = exposure * (exposure/10.0 + 1.0) / (exposure + 1.0);
  //float YD = exposure * (exposure/10.0 + 1.0) / (exposure + 1.0);
  //texel *= YD;
  Y = Y * exposure;
  Y = Y / (Y + 1.0);
  texel.rgb = texel.rgb * Y;
  //texel.rgb = texel.rgb / (texel.rgb + vec3(1.0, 1.0, 1.0));
  
  gl_FragColor = vec4(texel.rgb, 1.0);
  
  //if (texel.r == 0.0) {
    //gl_FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
  //} else {
    //gl_FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    //gl_FragColor = vec4(1.0 - exp2(-texel.rgb) /** color*/, 1.0/*texel.a * gl_FrontMaterial.diffuse.a*/);
  //}
}
