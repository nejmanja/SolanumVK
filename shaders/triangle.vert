#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 outColor;

void main()
{
    gl_Position = vec4(inPosition.xy, inPosition.z + 0.5f, 1.0);

    outColor = inColor;
}