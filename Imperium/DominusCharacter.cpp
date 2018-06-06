#include "DominusCharacter.h"

DominusCharacter::DominusCharacter()
{
}

DominusCharacter::DominusCharacter(std::string aName, DominusDevice& device, glm::vec3& position, glm::vec4 color) : DominusModel(device, position, color)
{
	name = aName;
}

DominusCharacter::~DominusCharacter()
{
}
