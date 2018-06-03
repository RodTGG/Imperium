#include "DominusCamera.h"
#include "DominusTools.h"
#include <iostream>
#include <gtc/matrix_transform.hpp>

DominusCamera::DominusCamera()
{
    radius = 10.0f;
    yaw = 90.0f;
    pitch = 0.0f;
    roll = 0.0f;
    rotationSpeed = 1.0f;
    movementSpeed = 1.0f;
    sensitivity = 0.1f;

	position = glm::vec3(0.0f);
	rotation = glm::vec3(0.0f);
    
	up = glm::vec3(0.0f, 0.0f, 1.0f);
    cameraFront = glm::vec3(0.0f, -1.0f, 0.0f);

    cameraTarget = glm::vec3(0.0f);
    cameraDirection = glm::normalize(position - cameraTarget);
    cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    cameraUp = glm::cross(cameraDirection, cameraRight);
}


DominusCamera::~DominusCamera()
{
}

void DominusCamera::update(double delta)
{
    deltaT = delta;
    updateViewMatrix();
}

void DominusCamera::updateViewMatrix()
{
	// TODO fix on camera update unknown rotation or translation

	//glm::mat4 rotm = glm::mat4(1.0f);
	//glm::mat4 transm = glm::translate(glm::mat4(0.0f), position);

	// Rotate pitch, yaw and roll (x,y,z)
	//rotm = glm::rotate(rotm, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	//rotm = glm::rotate(rotm, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	//rotm = glm::rotate(rotm, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	//view = rotm * transm;

    glm::vec3 front;
    front.x = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.z = -sin(glm::radians(pitch));

    cameraFront = glm::normalize(front);
    cameraRight = glm::normalize(glm::cross(cameraFront, up));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

	view = glm::lookAt(position, position + cameraFront, cameraUp);
}

void DominusCamera::setPerspective(float aFov, float aAspect, float aZnear, float aZfar)
{
	fov = aFov;
	znear = aZnear;
	zfar = aZfar;
	perspective = glm::perspective(glm::radians(fov), aAspect, znear, zfar);
	perspective[1][1] *= -1;
}

void DominusCamera::updateAspectRatio(float aAspect)
{
	perspective = glm::perspective(glm::radians(fov), aAspect, znear, zfar);
}

void DominusCamera::setTranslation(glm::vec3 aPosition)
{
	position = aPosition;
}

void DominusCamera::setRotation(glm::vec3 aRotation)
{
	rotation = aRotation;
}

void DominusCamera::rotate(glm::vec3 rotation)
{
	rotation += rotation;
	updateViewMatrix();
}

void DominusCamera::translate(glm::vec3 position)
{
	position += position;
	updateViewMatrix();
}

void DominusCamera::processKeyboardInput(cameraMovements direction)
{
	switch (direction)
	{
	case FOWARD:
		position += movementSpeed * cameraFront;
		break;
	case BACKWARD:
        position -= movementSpeed * cameraFront;
		break;
	case LEFT:
        position -= glm::normalize(glm::cross(cameraFront, cameraUp)) * movementSpeed;
		break;
	case RIGHT:
        position += glm::normalize(glm::cross(cameraFront, cameraUp)) * movementSpeed;
		break;
	}
}

void DominusCamera::processMouseInput(double xOffset, double yOffset)
{
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += static_cast<float>(xOffset);
    pitch += static_cast<float>(yOffset);

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
}

void DominusCamera::setLookAt(glm::vec3 center)
{
	view = glm::lookAt(position, center, up);
	updateViewMatrix();
}

std::ostream& operator<<(std::ostream & os, const DominusCamera & camera)
{
	os << "Camera details" << std::endl;
	os << "\tPosition: x=" << camera.position.x << " y=" << camera.position.y << " z=" << camera.position.z << std::endl;
	os << "\tRotation: x=" << camera.rotation.x << " y=" << camera.rotation.y << " z=" << camera.rotation.z << std::endl;
	os << "\tView: " << DominusTools::mat4ToString(camera.view) << std::endl;
	os << "\tPersp: " << DominusTools::mat4ToString(camera.perspective);

	return os;
}
