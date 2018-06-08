#include "DominusObject.h"
#include "DominusTools.h"
#include <iostream>

DominusObject::DominusObject(const glm::vec3 & aPosition, const glm::vec4 & aColor, const std::string file)
{
	position = aPosition;
	rotation = glm::vec3(1.0f);
	scaling = glm::vec3(1.0f);
	UVScaling = glm::vec2(1.0f);
	modelMat = glm::mat4(1.0f);
	color = aColor;
	model = new DominusModel(file);
}

DominusObject::~DominusObject()
{
	delete model;
}

void DominusObject::updateModelMatrix()
{
	glm::mat4 transMat = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 rotMat = glm::mat4(1.0f);
	rotMat = glm::rotate(rotMat, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotMat = glm::rotate(rotMat, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotMat = glm::rotate(rotMat, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scaling);
	modelMat = transMat * rotMat * scaleMat;
}

void DominusObject::draw(VkCommandBuffer* commandBuffer, VkPipelineLayout* layout)
{
	 vkCmdPushConstants(*commandBuffer, *layout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(glm::vec4(1.0f)), &color);
	 vkCmdPushConstants(*commandBuffer, *layout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::vec4(1.0f)), sizeof(glm::mat4(1.0f)), &modelMat);
	 vkCmdDrawIndexed(*commandBuffer, static_cast<uint32_t>(model->indices.size()), 1, 0, vertexOffset, 0);
}

void DominusObject::update(double deltaTime)
{
}

void DominusObject::prepare()
{
	model->loadFromFile();
	updateModelMatrix();
}

std::ostream & operator<<(std::ostream & os, DominusObject & dObject)
{
	os << "World info" << std::endl;
	os << "\tLocation x=" << dObject.position.x << " y=" << dObject.position.y << " z=" << dObject.position.z << std::endl;
	os << "\tRotation x=" << dObject.rotation.x << " y=" << dObject.rotation.y << " z=" << dObject.rotation.z << std::endl;
	os << "\tScaling x=" << dObject.scaling.x << " y=" << dObject.scaling.y << " z=" << dObject.scaling.z << std::endl;
	os << "\tUVScaling x=" << dObject.UVScaling.x << " y=" << dObject.UVScaling.y << std::endl;
	os << "Model Matrix " << std::endl;
	os << DominusTools::mat4ToString(dObject.modelMat) << std::endl;
	os << dObject.model;
	return os;
}