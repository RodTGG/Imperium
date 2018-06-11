#include "Agent.h"
#include "World.h"

Agent::Agent() : DominusObject()
{
}

Agent::Agent(World* world, const uint32_t aTeam, const glm::vec3 & position, const glm::vec4 & color, const std::string modelName) : DominusObject(world, position, color, modelName)
{
	team = aTeam;
}

Agent::~Agent()
{
}

void Agent::draw(VkCommandBuffer * commandBuffer, VkPipelineLayout * layout)
{
	DominusObject::draw(commandBuffer, layout);
}

void Agent::update(double deltaTime)
{
	auto tmp = static_cast<float>(deltaTime);
	force = calculate(deltaTime);
	force = glm::min(force, maxForce);
	accel = force / mass;
	vel += accel * tmp;
	vel = glm::min(vel, maxSpeed);

	position.x += vel.x * tmp;
	position.y += vel.y * tmp;

	auto start = glm::normalize(glm::vec3(0.f, 1.f, 0.f));
	auto end = glm::normalize(position);
	auto dot = glm::dot(start, end);
	glm::vec3 axis = glm::cross(start, end);
	
	auto rot1 = glm::quat(0.5f, axis.x, axis.y, axis.z);
	rotation = rot1;
	/*glm::vec3 right = glm::cross(position, glm::vec3(0.f, 0.f, 1.f));
	auto dUp = glm::cross(right, position);
	auto nUp = rot1 * glm::vec3(0.f, 0.f, 1.f);
	auto rot2 = */

	DominusObject::update(deltaTime);
}

glm::vec2 Agent::calculate(double deltaTime)
{
	if (team == 1)
	{
		for (auto p : world->players)
		{
			if (p != this)
			{
				return seek(p->position);
			}
		}
	}

	return glm::vec2();
}

float Agent::speed()
{
	return glm::length(vel);
}

glm::vec2 Agent::seek(glm::vec3 targetPos)
{
	glm::vec2 tPos(targetPos.x, targetPos.y);
	glm::vec2 sPos(position.x, position.y);

	auto desired_vel = glm::normalize(tPos - sPos) * maxSpeed;
	return desired_vel - vel;
}
