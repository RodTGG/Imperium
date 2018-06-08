#pragma once
#include "DominusModel.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

class DominusObject
{
public:
	DominusModel* model;

	// Model color
	glm::vec4 color;

	// Model matrix
	glm::mat4 modelMat;

	// Model position
	glm::vec3 position;

	// Model rotation
	glm::vec3 rotation;

	// Model scaling
	glm::vec3 scaling;

	// Offset in vertex buffer
	uint32_t vertexOffset;

	// Texture UV Scaling
	glm::vec2 UVScaling;

	DominusObject(const glm::vec3& position, const glm::vec4& color, const std::string file);
	~DominusObject();

	void updateModelMatrix();

	virtual void draw(VkCommandBuffer* commandBuffer, VkPipelineLayout* layout);
	virtual void update(double deltaTime);
	virtual void prepare();

	// Overload << operator
	friend std::ostream& operator<<(std::ostream& os, DominusModel& dModel);
};

