#include "Agent.h"


Agent::Agent(const int aTeam, const std::string filePath, const glm::vec3 & position, const glm::vec4 & color) : DominusObject(filePath, position, color)
{
	team = aTeam;
}

Agent::~Agent()
{
}