#include "DominusCamera.h"

DominusCamera::DominusCamera()
{
}


DominusCamera::~DominusCamera()
{
}

void DominusCamera::updateViewMatrix()
{
	glm::mat4 rotM = glm::mat4(1.0f);
	glm::mat4 transM;

	rotM = glm::rotate(rotM, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotM = glm::rotate(rotM, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotM = glm::rotate(rotM, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	transM = glm::translate(glm::mat4(1.0f), position);

	view = transM * rotM;
	view = glm::lookAt(position, center, glm::vec3(0.0f, 0.0f, 1.0f));
}

void DominusCamera::setPerspective(float aFov, float aAspect, float aZnear, float aZfar)
{
	fov = aFov;
	znear = aZnear;
	zfar = aZfar;
	perspective = glm::perspective(glm::radians(fov), aAspect, znear, zfar);
	perspective[1][1] *= -1;

	//glm::ortho(0.0f, (float)gSwapChainExtent.width, 0.0f, (float)gSwapChainExtent.height, 0.1f, 10.0f);
	/*ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), gSwapChainExtent.width / (float)gSwapChainExtent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;*/
}

void DominusCamera::updateAspectRatio(float aAspect)
{
	perspective = glm::perspective(glm::radians(fov), aAspect, znear, zfar);
}

void DominusCamera::setTranslation(glm::vec3 aPosition)
{
	position = aPosition;
	updateViewMatrix();
}

void DominusCamera::setRotation(glm::vec3 aRotation)
{
	rotation = aRotation;
	updateViewMatrix();
}

void DominusCamera::rotate(glm::vec3 aDelta)
{
	rotation += aDelta;
	updateViewMatrix();
}

void DominusCamera::translate(glm::vec3 aDelta)
{
	position += aDelta;
	updateViewMatrix();
}

void DominusCamera::setLookAt(glm::vec3 aCenter)
{
	center = aCenter;
	view = glm::lookAt(position, center, glm::vec3(0.0f, 0.0f, 1.0f));
}
