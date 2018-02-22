#include "DominusCamera.h"
#include "DominusTools.h"
#include <iostream>
#include <gtc/matrix_transform.hpp>

DominusCamera::DominusCamera()
{
	position = glm::vec3(0.0f);
	rotation = glm::vec3(0.0f);
}


DominusCamera::~DominusCamera()
{
}

void DominusCamera::updateViewMatrix()
{
	// TODO fix on camera update unknown rotation or translation

	glm::mat4 rotm = glm::mat4(1.0f);
	glm::mat4 transm = glm::translate(glm::mat4(1.0f), position * movementSpeed);

	rotm = glm::rotate(rotm, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotm = glm::rotate(rotm, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotm = glm::rotate(rotm, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	view = rotm * transm;
	//view = glm::lookAt(position, center, glm::vec3(0.0f, 0.0f, 1.0f));

	std::cout << *this << std::endl;
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

void DominusCamera::rotate(glm::vec3 aDelta)
{
	rotation += aDelta;
	updateViewMatrix();
}

void DominusCamera::translate(glm::vec3 aDelta)
{
	position += movementSpeed;
	updateViewMatrix();
}

void DominusCamera::setLookAt(glm::vec3 aCenter)
{
	center = aCenter;
	view = glm::lookAt(position, center, glm::vec3(0.0f, 0.0f, 1.0f));
}

std::ostream& operator<<(std::ostream & os, const DominusCamera & camera)
{
	os << "Camera details" << std::endl;
	os << "\tPosition: x=" << camera.position.x << " y=" << camera.position.y << " z=" << camera.position.z << std::endl;
	os << "\tRotation: x=" << camera.rotation.x << " y=" << camera.rotation.y << " z=" << camera.rotation.z << std::endl;
	os << "\tView: " << DominusTools::mat4ToString(camera.view) << std::endl;
	os << "\tPerspective: " << DominusTools::mat4ToString(camera.perspective);

	return os;
}
