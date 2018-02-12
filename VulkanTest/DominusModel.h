#pragma once
#include "DominusBuffer.h"
#include "DominusDevice.h"
#include "glm.hpp"
#include "Vertex.h"

#include <vector>

class DominusModel
{
public:
	// TODO use pointers to single buffer to remove inefficient buffer switching
	
	// Pointer to vertex buffer
	DominusBuffer * vBuffer;

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

	DominusModel();
	DominusModel(DominusDevice& device, DominusBuffer& vertexBuffer, const glm::vec3& position);
	~DominusModel();

	// Loads vertices and indices to vector
	bool loadFromFile(const std::string fPath);

	// Overload << operator
	friend std::ostream& operator<<(std::ostream& os, DominusModel& dModel);
private:
};