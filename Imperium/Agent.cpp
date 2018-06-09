#include "Agent.h"

Agent::Agent(World* world, const int aTeam, const glm::vec3 & position, const glm::vec4 & color, const std::string modelName) : DominusObject(world, position, color, modelName)
{
	team = aTeam;
}

Agent::~Agent()
{
}