/*uniform vec4 ambientColour;
uniform vec4 diffuseColour;
uniform vec4 specularColour;
uniform sampler2D colourMap; // This is the original texture
uniform sampler2D normalMap; // This is the normal-mapped version of our texture
 
smooth in vec3 vVaryingNormal;
smooth in vec3 vVaryingLightDir;
smooth in vec2 vTexCoords;*/
uniform sampler2D tex;
uniform sampler2D bump;

//varying vec4 diffuse, ambient;
varying vec3 vVaryingNormal;
varying vec3 vVaryingLightDir;
varying vec2 vTexCoords;
varying vec2 vBumpCoords;
 
//out vec4 vFragColour;
 
void main(void)
{ 
	// Create a normal which is our standard normal + the normal map perturbation (which is going to be either positive or negative)
	vec3 normalAdjusted = vVaryingNormal + normalize((texture2D(bump, vBumpCoords.st).rgb - 0.5)*2.0);
 
	// Calculate diffuse intensity
	float diffuseIntensity = max(0.0, dot(normalize(normalAdjusted), normalize(vVaryingLightDir)));
 
	// Add the diffuse contribution blended with the standard texture lookup and add in the ambient light on top
  vec3 color = diffuseIntensity * (gl_FrontMaterial.diffuse).rgb + (gl_FrontMaterial.ambient).rgb;
  vec4 texel = texture2D(tex, vTexCoords.st); //vec4(1.0, 1.0, 1.0, 1.0);
	gl_FragColor = vec4(texel.rgb * color, texel.a * gl_FrontMaterial.diffuse.a);
 
 //diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
  //ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
  //ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;
 
	// Set the almost final output color as a vec4 - only specular to go!
	//vFragColour = vec4(colour, 1.0);
 
	// Calc and apply specular contribution
	//vec3 vReflection        = normalize(reflect(-normalize(normalAdjusted), normalize(vVaryingLightDir)));
	//float specularIntensity = max(0.0, dot(normalize(normalAdjusted), vReflection));
 
	// If the diffuse light intensity is over a given value, then add the specular component
	// Only calc the pow function when the diffuseIntensity is high (adding specular for high diffuse intensities only runs faster)
	// Put this as 0 for accuracy, and something high like 0.98 for speed
	//if (diffuseIntensity > 0.98)
	//{
		//float fSpec = pow(specularIntensity, 64.0);
		//vFragColour.rgb += vec3(fSpec * specularColour.rgb);
	//}
}
