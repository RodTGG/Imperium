#pragma once
#include "DominusTools.h"
#include "Agent.h"
#include "DominusDevice.h"

#include <vector>
#include <array>
#include <string>

class World
{
public:
	DominusDevice* device;

	std::vector<Agent*> players;
	//std::vector<DominusModel*> sceneModels;

	double moveTime = 0;

	World();
	World(DominusDevice& device);
	~World();

	void loadWorld();
	void update(double detlaTime);
};

