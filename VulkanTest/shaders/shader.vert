#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject 
{
	mat4 model;
	mat4 view;
	mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
//layout(location = 3) in vec3 lightColor;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
	mat4 MVPmat = ubo.proj * ubo.view * ubo.model;
    gl_Position = MVPmat * vec4(inPosition, 1.0);
	fragColor = vec3(1.0);
    //fragColor = vec4(lightColor * inColor, 1.0);
	fragTexCoord = inTexCoord;
}