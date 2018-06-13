#include "Agent.h"
#include "World.h"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>
#include <limits>

Agent::Agent() : DominusObject()
{
}

Agent::Agent(World* world, const uint32_t aTeam, MODES aMode, const glm::vec3 & position, const glm::vec4 & aColor, const std::string modelName) : DominusObject(world, position, aColor, modelName)
{
	team = aTeam;
	mode = aMode;

	if (team == 1)
		color = glm::vec4(1.f, 0.f, 0.f, 1.f);
	else if (team == 2)
		color = glm::vec4(0.f, 0.f, 1.f, 1.f);

	switch (mode)
	{
	case DEFAULT:
		model = "invader";
		hp = 50;
		break;
	case BASE:
		model = "base";
		scaling = glm::vec3(3.f);
		break;
	case BARRACKS:
		model = "barracks";
		scaling = glm::vec3(4.f);
		hp = 80;
		break;
	case TOWER:
		model = "tower";
		scaling = glm::vec3(4.f);
		break;
	case UNIT:
		model = "invader";
		scaling = glm::vec3(0.5f);
		hp = 20;
		break;
	case BULLET:
		model = "circle";
		hp = 1;
		damage = 3;
		break;
	case RANGED_UNIT:
		model = "invader";
		hp = 10;
		scaling = glm::vec3(0.3f, 0.7f, 0.5f);
		break;
	default:
		model = "invader";
		break;
	}
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

	if (hp <= 0)
		alive = false;

	if (alive)
	{
		checkCollision();

		switch (mode)
		{
		case Agent::DEFAULT:
			updatePhysics(fDelta);
			break;
		case UNIT:
			updatePhysics(fDelta);
			break;
		case RANGED_UNIT:
			updatePhysics(fDelta);
			break;
		case BULLET:
			continueOnHeading();
			updatePhysics(fDelta);
			if (position.x > 200.f || position.y > 200.f || position.z > 200.f)
				alive = false;
			break;
		case Agent::BASE:
			break;
		case Agent::BARRACKS:
			spawnUnit(fDelta);
			break;
		case Agent::TOWER:
			break;
		default:
			break;
		}
	}

	DominusObject::update(deltaTime);
}

void Agent::updatePhysics(float fDelta)
{
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
}

void Agent::checkCollision()
{
	for (auto p : world->players)
	{
		if (p != this && p->team != team)
		{
			auto mDistance = glm::distance(p->position, position);

			if (mDistance < collisionRadius)
			{
				p->hp -= damage;
				hp -= p->damage;
			}
		}
	}
}

glm::vec2 Agent::calculate(float delta)
{
	switch (mode)
	{
	case DEFAULT:
		collectionElapsed += delta;
		return mineMineral();
		break;
	case UNIT:
		return pursuit(*getClosestEnemyUnit());
		break;
	case RANGED_UNIT:
		return rangedBehavior(delta);
		break;
	case BULLET:
		return continueOnHeading();
		break;
	default:
		break;
	}

	return glm::vec2(0.f);
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

glm::vec2 Agent::pursuit(Agent & evader)
{
	auto toEvader = evader.position - position;
	auto relativeHeading = glm::dot(rotation, evader.rotation);

	if (glm::dot(glm::quat(toEvader), rotation) > 0 && relativeHeading < 0.95)
		return arrive(evader.position, decelSpeeds[0]);

	auto lookAheadTime = glm::length(toEvader) / (maxSpeed + evader.getSpeed());
	lookAheadTime += (1 - relativeHeading) * -1;
	auto lookAheadPos = evader.position + glm::vec3(evader.vel, 0.f) * lookAheadTime;

	return arrive(lookAheadPos, decelSpeeds[0]);
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

glm::vec2 Agent::mineMineral()
{
	Mineral* closest = nullptr;
	auto distanceToClosest = std::numeric_limits<float>::max();

	for (auto m : world->minerals)
	{
		if (!m->depleted) 
		{
			auto mDistance = glm::distance(m->position, position);
			if (mDistance < distanceToClosest)
			{
				closest = m;
				distanceToClosest = mDistance;
			}
		}
	}

	if (closest)
	{
		if (distanceToClosest <= 5.f && collectionElapsed > collectionTime)
		{
			resource += closest->collectMineral();
			std::cout << "Team " << team << "- has " << resource << std::endl;
			collectionElapsed = 0;
		}

		return arrive(closest->position, decelSpeeds[0]);
	}

	return arrive(getTeamType(BASE)->position, decelSpeeds[0]);
}

glm::vec2 Agent::rangedBehavior(float fDelta)
{
	shootTime += fDelta;

	if (glm::distance(getClosestEnemyUnit()->position, position) > 30.f && shootTime > shootRate) 
	{
		auto bullet = new Agent(world, team, Agent::BULLET, position);
		bullet->vel = bullet->calculateBullet(bullet->getClosestEnemyUnit());
		world->players.push_back(bullet);
		shootTime = 0;
	}

	return glm::vec2();
}

glm::vec2 Agent::calculateBullet(Agent* target)
{
	auto toTarget = glm::distance(position, target->position) * maxSpeed;
	auto myPos = (position - target->position) * toTarget;
	auto targetPos = (target->position + glm::vec3(target->vel, 0.f)) * toTarget;

	return glm::normalize(targetPos - myPos) * maxSpeed;
}

glm::vec2 Agent::continueOnHeading()
{
	return force;
}

Agent* Agent::getClosestEnemyUnit()
{
	Agent* closest = nullptr;
	auto distanceToClosest = std::numeric_limits<float>::max();

	for (auto p : world->players)
	{
		if (p != this && p->team != team && p->mode != BULLET)
		{
			auto pDistance = glm::distance(p->position, position);
			if (pDistance < distanceToClosest)
			{
				closest = p;
				distanceToClosest = pDistance;
			}
		}
	}

	return closest;
}

Agent * Agent::getEnemyType(MODES mode)
{
	for (auto p : world->players)
		if (p->team != team && p->mode == mode)
			return p;

	return nullptr;
}

Agent * Agent::getTeamType(MODES mode)
{
	for (auto p : world->players)
		if (p->team == team && p->mode == mode)
			return p;

	return nullptr;
}

void Agent::spawnUnit(float delta)
{
	auto builder = getTeamType(DEFAULT);

	if (!builder)
		return;

	spawnElapsed += delta;

	if (spawnElapsed > spawnTimer && builder->resource >= unitCost)
	{
		builder->resource -= unitCost;
		world->players.push_back(new Agent(world, team, UNIT, position));
		spawnElapsed = 0.f;
	}
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
