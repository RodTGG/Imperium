#pragma once
#include "DominusBuffer.h"
#include <vulkan\vulkan.hpp>
#include <glm.hpp>

class DominusModel
{
public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scaling;
	glm::vec2 UVScaling;
	DominusBuffer vBuffer;
	DominusBuffer iBuffer;

	DominusModel();
	DominusModel(glm::vec3 position);
	~DominusModel();
private:

};

