#pragma once
#include <vulkan\vulkan.h>
#include <glm.hpp>

class DominusModel
{
public:
	DominusModel();
	~DominusModel();

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scaling;
private:

};

