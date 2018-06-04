#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

layout (push_constant) uniform PushConstant
{
	vec4 color;
} pushConstant;

void main() {
    outColor = pushConstant.color;
}