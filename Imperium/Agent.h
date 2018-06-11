#pragma once
#include "DominusObject.h"

class Agent : public DominusObject
{
public:
	enum AGENT_MODES
	{
		MINING,
		RESTING,
		SHOOTING,
		WANDERING,
	};

	uint32_t team;

	// Physics and limits
	float decelSpeeds[3] = { 0.9f, 0.5f, 0.2f };
	float maxSpeed = 10.0f;
	float maxForce = 1000.0f;
	float mass = 1.0;

	// Wander details
	glm::vec2 wanderTarget = glm::vec2(1.f, 0.f);
	float wanderDist = 1.f;
	float wanderRadius = 1.f;
	float wanderJitter = 20.f;

	// Physics Vectors
	glm::vec2 vel;
	glm::vec2 heading;
	glm::vec2 force;
	glm::vec2 accel;

	Agent();
	Agent(World* world, const uint32_t team, const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec4& color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), const std::string modelName = "invader");
	~Agent();

	void draw(VkCommandBuffer* commandBuffer, VkPipelineLayout* layout) override;
	void update(double deltaTime) override;

	float getSpeed();
	glm::vec2 calculate(float delta);

	glm::vec2 seek(glm::vec3 targetPos);
	glm::vec2 flee(glm::vec3 targetPos);
	glm::vec2 arrive(glm::vec3 targetPos, float decelSpeed);
	glm::vec2 pursuit();
	glm::vec2 wander(float delta);

	// TODO: Look for easier and more accurate way, maybe in built GLM function
	// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/
	glm::quat rotationBetweenVectors(glm::vec3 start, glm::vec3 dest);
	float randomUniform(float min, float max);
};

