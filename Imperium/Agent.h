#pragma once
#include "DominusObject.h"

class Agent : public DominusObject
{
public:
	double moveTime = 0.0;

	int team = 0;

	Agent(const int team, const std::string filePath = "invader.obj", const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec4& color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    ~Agent();
};

