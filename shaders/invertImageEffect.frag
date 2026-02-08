#version 460

layout(location = 0) in vec2 inUvs;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D _inputImage;

void main() {
    vec4 inputImageColor = texture(_inputImage, inUvs);
    outColor = vec4(vec3(1.0) - inputImageColor.rgb, 1.0);
}
