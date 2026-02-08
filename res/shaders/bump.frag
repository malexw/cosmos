#version 150

uniform sampler2D tex;
uniform sampler2D bump;
uniform vec3 lightPosEye;

in vec2 vTexCoords;
in vec2 vBumpCoords;
in vec3 vMatDiffuse;
in mat3 vTBN;

out vec4 fragColor;

void main(void)
{
	vec3 normalTangent = normalize(texture(bump, vBumpCoords.st).rgb * 2.0 - 1.0);
	vec3 normalEye = normalize(vTBN * normalTangent);
	float diffuseIntensity = max(0.0, dot(normalEye, -normalize(lightPosEye)));

	vec3 color = (0.1 + diffuseIntensity) * vMatDiffuse;
	vec4 texel = texture(tex, vTexCoords.st);
	fragColor = vec4(texel.rgb * color, texel.a);
}
