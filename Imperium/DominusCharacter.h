#pragma once
#include "DominusModel.h"

class DominusCharacter : public DominusModel
{
public:
    DominusCharacter(DominusDevice& device, const glm::vec3& position = glm::vec3(0));
    ~DominusCharacter();
};

