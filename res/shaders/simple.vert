//varying vec3 lightDir;
varying vec4 diffuse, ambient;
varying vec3 normal, lightDir, halfVec;

void main()
{	
	//vec3 lightdir = normalize(vec3(gl_LightSource[0].position));
  //intensity = dot(lightdir, gl_Normal);
  
  //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  //normal = gl_NormalMatrix * gl_Normal;
  //lightDir = vec3(gl_LightSource[0].position) * gl_ModelViewProjectionMatrix;
  
  vec3 normal, lightDir;
  vec4 diffuse, ambient, globalAmbient;
  //float NdotL;
  
  normal = normalize(gl_NormalMatrix * gl_Normal);
  lightDir = normalize(vec3(gl_LightSource[0].position));
  halfVec = normalize(gl_LightSource[0].halfVector.xyz);
  
  //NdotL = max(dot(normal, lightDir), 0.0);
  
  diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
  ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
  ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;
  
  //gl_FrontColor = NdotL * diffuse + ambient + globalAmbient;
  
  gl_Position = ftransform();
}
