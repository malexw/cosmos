//varying vec3 lightDir;
varying vec4 diffuse, ambient;
varying vec3 normal, lightDir, halfVec;

void main()
{ 
  /*float intensity = dot(vec3(gl_LightSource[0].position), normalize(normal));
  
  if (intensity > 0.95)
    gl_FragColor = vec4(1.0, 0.5, 0.5, 1.0);
  else if (intensity > 0.5)
    gl_FragColor = vec4(0.6, 0.3, 0.3, 1.0);
  else if (intensity > 0.25)
    gl_FragColor = vec4(0.4, 0.2, 0.2, 1.0);
  else
    gl_FragColor = vec4(0.2, 0.1, 0.1, 1.0);*/
  //gl_FragColor = vec4(0.8,0.4,0.8,1.0);
  vec3 norm, halfV;
  float nDotL, nDotHv;
  vec4 color = ambient;
  
  norm = normalize(normal);
  nDotL = max(dot(norm, lightDir), 0.0);
  
  if (nDotL > 0.0) {
    color += diffuse * nDotL;
    halfV = normalize(halfVec);
    nDotHv = max(dot(norm, halfV), 0.0);
    color += gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(nDotHv, gl_FrontMaterial.shininess);
  }
  
  gl_FragColor = color;
  //gl_FragColor = gl_Color;
}
