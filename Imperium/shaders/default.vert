#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform MVP 
{
	//mat4 model;
	mat4 view;
	mat4 proj;
} mvp;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout (push_constant) uniform TransformationMat
{
	layout(offset = 16) mat4 trans;
} transMat;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
	mat4 MVPmat = mvp.proj * mvp.view * transMat.trans;
	//MVPmat *= transMat.trans;
    gl_Position = MVPmat * vec4(inPosition, 1);

	fragColor = vec3(1.0);
	fragTexCoord = inTexCoord;
}