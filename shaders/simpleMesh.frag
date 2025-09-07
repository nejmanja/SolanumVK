#version 460

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec3 inSunDirection;
layout(location = 2) in vec3 inSunColor;

layout(location = 0) out vec4 outColor;


void main()
{
	outColor = vec4(inColor * inSunColor, 1.0);
}