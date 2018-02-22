#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm.hpp>
#include <ostream>

class DominusCamera
{
public:
	glm::mat4 perspective;
	glm::mat4 view;
	glm::vec3 rotation;
	glm::vec3 position;

	float rotationSpeed = 1.0f;
	float movementSpeed = 1.0f;

	DominusCamera();
	~DominusCamera();

	void updateViewMatrix();
	void setPerspective(float fov, float aspect, float znear, float zfar);
	void updateAspectRatio(float aspect);
	void setTranslation(glm::vec3 position);
	void setRotation(glm::vec3 rotation);
	void rotate(glm::vec3 delta);
	void translate(glm::vec3 delta);
	void setLookAt(glm::vec3 center);

	friend std::ostream& operator<<(std::ostream& os, const DominusCamera& camera);
private:
	float fov;
	float znear, zfar;
	glm::vec3 center;
};

