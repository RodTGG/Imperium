#include "Agent.h"
#include "World.h"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

Agent::Agent() : DominusObject()
{
}

Agent::Agent(World* world, const uint32_t aTeam, const glm::vec3 & position, const glm::vec4 & aColor, const std::string modelName) : DominusObject(world, position, aColor, modelName)
{
	team = aTeam;

	if (team == 1)
		color = glm::vec4(1.f, 0.f, 0.f, 1.f);
	else if (team == 2)
		color = glm::vec4(0.f, 0.f, 1.f, 1.f);
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
	auto fDelta = static_cast<float>(deltaTime);

	force = calculate(fDelta);
	force = glm::min(force, maxForce);
	accel = force / mass;
	vel += accel * fDelta;
	vel = glm::min(vel, maxSpeed);

	position.x += vel.x * fDelta;
	position.y += vel.y * fDelta;

	if (glm::length(vel) > 0.001f)
	{
		auto rot1 = rotationBetweenVectors(glm::vec3(0.f, 1.f, 0.f), position);
		glm::vec3 right = glm::cross(position, glm::vec3(0.f, 0.f, 1.f));
		auto dUp = glm::cross(right, position);
		auto nUp = rot1 * glm::vec3(0.f, 0.f, 1.f);
		auto rot2 = rotationBetweenVectors(nUp, dUp);
		rotation = rot2 * rot1;
	}

	DominusObject::update(deltaTime);
}

glm::vec2 Agent::calculate(float delta)
{
	if (team == 1)
	{
		//return wander(delta);

		for (auto p : world->players)
		{
			if (p != this)
			{
				return arrive(p->position, decelSpeeds[2]);
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

glm::vec2 Agent::flee(glm::vec3 targetPos)
{
	auto panicDistance = 100.f;
	auto difference = glm::distance(position, targetPos);

	if (difference > panicDistance)
		return glm::vec2();

	auto desiredVel = glm::normalize(position - targetPos) * maxSpeed;
	auto result = desiredVel - glm::vec3(vel, 0.f);

	return glm::vec2(result.x, result.y);
}

glm::vec2 Agent::arrive(glm::vec3 targetPos, float decelSpeed)
{
	auto toTarget = targetPos - position;
	auto dist = glm::length(toTarget);

	if (dist > 0.f) 
	{
		auto speed = dist / decelSpeed;
		speed = glm::min(speed, maxSpeed);
		auto dVel = toTarget * (speed / dist);
		glm::vec2 dVel2(dVel.x, dVel.y);
		return (dVel2 - vel);
	}

	return glm::vec2(0.f);
}

glm::vec2 Agent::wander(float delta)
{
	auto wt = wanderTarget;
	auto jitter_tts = wanderJitter * delta;
	wt += glm::vec2(randomUniform(-1.0, 1.0) * jitter_tts, randomUniform(-1.0, 1.0) * jitter_tts);
	glm::normalize(wt);
	wt *= wanderRadius;
	auto target = glm::vec3(wt, 0.f) + glm::vec3(wanderDist, 0.f, 0.f);
	
	auto tmp = glm::translate(modelMat, target);
	auto tmp2 = tmp[3];

	return seek(tmp2);
}

glm::quat Agent::rotationBetweenVectors(glm::vec3 start, glm::vec3 dest)
{
	start = normalize(start);
	dest = normalize(dest);

	float cosTheta = dot(start, dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f) {
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
		if (glm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = normalize(rotationAxis);
		return glm::angleAxis(glm::radians(180.0f), rotationAxis);
	}

	rotationAxis = cross(start, dest);

	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return glm::quat(s * 0.5f, rotationAxis.x * invs, rotationAxis.y * invs, rotationAxis.z * invs);
}

float Agent::randomUniform(float min, float max)
{
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}
