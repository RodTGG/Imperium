#pragma once
class World;

#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vulkan/vulkan.hpp>

class DominusObject
{
public:
	World * world;

	std::string model;

	// Model color
	glm::vec4 color;

	// Model matrix
	glm::mat4 modelMat;

	// Model position
	glm::vec3 position;

	// Model rotation
	glm::quat rotation;

	// Model scaling
	glm::vec3 scaling;

	// Texture UV Scaling
	glm::vec2 UVScaling;

	DominusObject();
	DominusObject(World* world, const glm::vec3& position, const glm::vec4& color, const std::string modelName);
	~DominusObject();

	void updateModelMatrix();

	virtual void draw(VkCommandBuffer* commandBuffer, VkPipelineLayout* layout);
	virtual void update(double deltaTime);
	virtual void prepare();

	// Overload << operator
	friend std::ostream& operator<<(std::ostream& os, DominusObject& dObject);
};

