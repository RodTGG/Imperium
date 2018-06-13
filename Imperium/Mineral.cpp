#include "Mineral.h"

Mineral::Mineral(World* aWorld) : DominusObject(aWorld, glm::vec3(0.f), glm::vec4(0.f, 0.f, 0.6f, 1.f), "mineral")
{
	collectionAmount = 10;
	mineralAmount = 200;
	depleted = false;
	scaling = glm::vec3(5.f);
}

Mineral::Mineral(World* aWorld, glm::vec3 aPosition, uint32_t amount, uint32_t collectAmount) : DominusObject(aWorld, aPosition, glm::vec4(0.3f, 0.2f, 0.4f, 1.f), "mineral")
{
	depleted = false;
	collectionAmount = collectAmount;
	mineralAmount = amount;
	scaling = glm::vec3(5.f);
}

Mineral::~Mineral()
{
}

void Mineral::update(double deltaTime)
{
	/*if (!mineralAmount && !depleted) 
	{
		depleted = true;
	}*/
}

uint32_t Mineral::collectMineral()
{
	if (mineralAmount > collectionAmount) 
	{
		mineralAmount -= collectionAmount;

		if (!mineralAmount)
			depleted = true;

		return collectionAmount;
	}
	else 
	{
		auto result = mineralAmount % collectionAmount;
		mineralAmount -= collectionAmount;

		if (!mineralAmount)
			depleted = true;

		return result;
	}
}
