#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include "DominusBuffer.h"
#include "DominusDevice.h"
#include "glm.hpp"
#include "Vertex.h"

#include <vector>

class DominusModel
{
public:
	glm::vec4 color;

	glm::mat4 modelMat;

	// TODO use pointers to single buffer to remove inefficient buffer switching
	
	// Pointer to vertex buffer
	//DominusBuffer * vBuffer;

	// Pointer to dominus device
	DominusDevice* device;

	// Model position
	glm::vec3 position;

	// Model rotation
	glm::vec3 rotation;

	// Model scaling
	glm::vec3 scaling;

	// Texture UV Scaling
	glm::vec2 UVScaling;

	// Unique model vertices
	std::vector<Vertex> vertices;

	// Model indices
	std::vector<uint32_t> indices;

	// Path to model file
	std::string filePath;

	// Offset in vertex buffer
	uint32_t vertexOffset;

	DominusModel();
	DominusModel(DominusDevice& device, const glm::vec3 position = glm::vec3(0.0f), const glm::vec4 color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	~DominusModel();

	void updateModelMatrix();

	virtual void draw(VkCommandBuffer* buffer);

	// Loads vertices and indices to vector
	virtual bool loadFromFile(const std::string fPath);

	// Overload << operator
	friend std::ostream& operator<<(std::ostream& os, DominusModel& dModel);
private:
};