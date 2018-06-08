#include "Agent.h"


Agent::Agent(const int aTeam, const glm::vec3 & position, const glm::vec4 & color, const std::string filePath) : DominusObject(position, color, filePath)
{
	team = aTeam;
}

Agent::~Agent()
{
}