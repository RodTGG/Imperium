#pragma once
#include "DominusObject.h"

class Mineral :
	public DominusObject
{
public:
	uint32_t collectionAmount;
	uint32_t mineralAmount;
	bool depleted;

	Mineral(World* world);
	Mineral(World* world, glm::vec3 position, uint32_t amount, uint32_t collectAmount);
	~Mineral();

	void update(double deltaTime) override;

	uint32_t collectMineral();
};

