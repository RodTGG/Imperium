#pragma once
#include "DominusObject.h"
#include <gtx/quaternion.hpp>

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

	float speed();

	glm::vec2 calculate(double deltaTime);
	glm::vec2 seek(glm::vec3 targetPos);

	glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest) {
		start = glm::normalize(start);
		dest = glm::normalize(dest);

		float cosTheta = dot(start, dest);
		glm::vec3 rotationAxis;

		if (cosTheta < -1 + 0.001f) {
			// special case when vectors in opposite directions:
			// there is no "ideal" rotation axis
			// So guess one; any will do as long as it's perpendicular to start
			rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
			if (glm::gtx::norm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
				rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

			rotationAxis = normalize(rotationAxis);
			return gtx::quaternion::angleAxis(glm::radians(180.0f), rotationAxis);
		}

		rotationAxis = cross(start, dest);

		float s = sqrt((1 + cosTheta) * 2);
		float invs = 1 / s;

		return glm::quat(
			s * 0.5f,
			rotationAxis.x * invs,
			rotationAxis.y * invs,
			rotationAxis.z * invs
		);

	}
};

