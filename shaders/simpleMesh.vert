#version 460

layout(set = 0, binding = 0) uniform SceneData{
    mat4 view;
    mat4 proj;
    vec3 sunDirection;
    vec3 sunColor;
} sceneData;

layout(set = 1, binding = 0) uniform Transform{
    mat4 model;
} transform;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec3 outSunDirection;
layout(location = 2) out vec3 outSunColor;

void main()
{
    gl_Position = sceneData.proj * sceneData.view * transform.model * vec4(inPosition, 1.0);

    outColor = inColor;
    outSunDirection = sceneData.sunDirection;
    outSunColor = sceneData.sunColor;
}