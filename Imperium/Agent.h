#pragma once
#include "DominusObject.h"

class Agent : public DominusObject
{
public:
	uint32_t team;
	float maxSpeed = 10.0f;
	float maxForce = 1000.0f;
	float mass = 1.0;

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

	glm::vec2 calculate(double deltaTime);
	glm::vec2 seek(glm::vec3 targetPos);

	// TODO: Look for easier and more accurate way, maybe in built GLM function
	// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/
	glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);
};

