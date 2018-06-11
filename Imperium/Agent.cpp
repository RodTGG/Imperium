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
	// Convert to deltaTime to float
	auto fTime = static_cast<float>(deltaTime);

	force = calculate(deltaTime);
	force = glm::min(force, maxForce);
	accel = force / mass;
	vel += accel * fTime;
	vel = glm::min(vel, maxSpeed);

	position.x += vel.x * fTime;
	position.y += vel.y * fTime;

	if (glm::length(vel) > 0.001f)
	{
		auto rot1 = RotationBetweenVectors(glm::vec3(0.f, 1.f, 0.f), position);
		glm::vec3 right = glm::cross(position, glm::vec3(0.f, 0.f, 1.f));
		auto dUp = glm::cross(right, position);
		auto nUp = rot1 * glm::vec3(0.f, 0.f, 1.f);
		auto rot2 = RotationBetweenVectors(nUp, dUp);
		rotation = rot2 * rot1;
	}

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

float Agent::getSpeed()
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

glm::quat Agent::RotationBetweenVectors(glm::vec3 start, glm::vec3 dest)
{
	glm::vec3 rotationAxis;

	start = glm::normalize(start);
	dest = glm::normalize(dest);

	rotationAxis = cross(start, dest);
	float cosTheta = dot(start, dest);
	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return glm::quat(s * 0.5f, rotationAxis.x * invs, rotationAxis.y * invs, rotationAxis.z * invs);
}
