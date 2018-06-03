#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <GLFW\glfw3.h>
#include <glm.hpp>
#include <ostream>

enum cameraMovements {
	FOWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class DominusCamera
{
public:
	// Camera perspective matrix
	glm::mat4 perspective;

	// Camera view matrix
	// right, up, front and position colums
	glm::mat4 view;

	glm::vec3 rotation;
	glm::vec3 position;

	glm::vec3 cameraTarget;
	glm::vec3 up;

	glm::vec3 cameraDirection;
	glm::vec3 cameraRight;
    glm::vec3 cameraFront;
	glm::vec3 cameraUp;

    float radius;
    float yaw;
    float pitch;
    float roll;
    float rotationSpeed;
    float movementSpeed;
    float sensitivity;
    float fov;
    float znear, zfar;
    double deltaT;

	DominusCamera();
	~DominusCamera();

    void update(double delta);

    // Creates a new view matrix with current position and rotation variables
    void updateViewMatrix();
	void setPerspective(float fov, float aspect, float znear, float zfar);
	void updateAspectRatio(float aspect);
	void setTranslation(glm::vec3 position);
	void setRotation(glm::vec3 rotation);
	void rotate(glm::vec3 rotation);
	void translate(glm::vec3 position);

	void processKeyboardInput(cameraMovements direction);
    void processMouseInput(double xOffset, double yOffset);

	void setLookAt(glm::vec3 center);

	friend std::ostream& operator<<(std::ostream& os, const DominusCamera& camera);
private:
};