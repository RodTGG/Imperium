#pragma once
#include "DominusObject.h"

class Agent : public DominusObject
{
public:
	int team = 0;

	Agent(World* world, const int team, const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec4& color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), const std::string modelName = "invader");
    ~Agent();
};

