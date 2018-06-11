#pragma once
#include "Agent.h"
#include "Mineral.h"
#include "DominusModel.h"
#include <string>
#include <unordered_map>
#include <vector>

class World
{
public:
	std::unordered_map<std::string, DominusModel*> models;
	std::vector<Agent*> players;
	std::vector<Mineral*> minerals;

	World();
	~World();

	void loadWorld();

	void draw(VkCommandBuffer * commandBuffer, VkPipelineLayout * layout);
	void update(double detlaTime);
	void processInput(int key, int action);

	void addModel(std::string modelName, DominusModel* model);
	uint32_t getModelVertexOffset(std::string modelName);
	uint32_t getModelIndexOffset(std::string modelName);
	uint32_t getModelVerticeCount(std::string modelName);
	uint32_t getModelIndexCount(std::string modelName);
};

