#version 150

uniform vec4 lightSpecular;
uniform vec4 matSpecular;
uniform float matShininess;

in vec4 diffuse, ambient;
in vec3 vNormal, lightDir, halfVec;

out vec4 fragColor;

void main()
{
  vec3 norm, halfV;
  float nDotL, nDotHv;
  vec4 color = ambient;

  norm = normalize(vNormal);
  nDotL = max(dot(norm, lightDir), 0.0);

  if (nDotL > 0.0) {
    color += diffuse * nDotL;
    halfV = normalize(halfVec);
    nDotHv = max(dot(norm, halfV), 0.0);
    color += matSpecular * lightSpecular * pow(nDotHv, max(matShininess, 1.0));
  }

  fragColor = color;
}
