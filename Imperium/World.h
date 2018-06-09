#pragma once
#include "Agent.h"
#include "DominusDevice.h"
#include "DominusModel.h"
#include <string>
#include <unordered_map>
#include <vector>

class World
{
public:
	DominusDevice* device;

	std::unordered_map<std::string, DominusModel*> models;
	std::vector<Agent*> players;

	double moveTime = 0;
	double waitTime = 2.0;
	double xOffset = 40.0f;

	World();
	World(DominusDevice& device);
	~World();

	void loadWorld();
	void update(double detlaTime);

	void addModel(std::string modelName, DominusModel* model);
	uint32_t getModelVertexOffset(std::string modelName);
	uint32_t getModelIndexOffset(std::string modelName);
	uint32_t getModelVerticeCount(std::string modelName);
	uint32_t getModelIndexCount(std::string modelName);
};

