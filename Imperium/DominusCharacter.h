#pragma once
#include "DominusModel.h"

class DominusCharacter : public DominusModel
{
public:
	std::string name;
	glm::vec4 color;

	DominusCharacter();
    DominusCharacter(std::string aName, DominusDevice& device, glm::vec3& position, glm::vec4 color);
    ~DominusCharacter();
};

