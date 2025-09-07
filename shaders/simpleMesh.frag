#version 460

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inSunDirection;
layout(location = 3) in vec3 inSunColor;

layout(location = 0) out vec4 outColor;


void main()
{
	float NdotL = clamp(dot(normalize(inNormal), normalize(inSunDirection)), 0.0f, 1.0f);
	vec3 diffuse = inColor * inSunColor * NdotL;
	vec3 ambient = inColor * 0.3;
	outColor = vec4(ambient + diffuse, 1.0);
}