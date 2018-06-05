#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform MVP 
{
	mat4 model;
	mat4 view;
	mat4 proj;
} mvp;

layout(binding = 2) uniform Transform
{
	mat4 transformation;
} transform;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
	mat4 MVPmat = mvp.proj * mvp.view * mvp.model;
	mat4 transfMat = MVPmat * transform.transformation;
    gl_Position = transfMat * vec4(inPosition, 1);

	fragColor = vec3(1.0);
	fragTexCoord = inTexCoord;
}