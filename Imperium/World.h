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
	// World properties
	int winner = -1;
	bool won = false;
	bool paused = false;

	// World players
	std::vector<Agent*> players;

	// World minerals
	std::vector<Mineral*> minerals;
	
	// Unique world models
	std::unordered_map<std::string, DominusModel*> models;

	World();
	~World();

	// Loads world and sets up models
	void loadWorld();

	// Draws all objects in the world
	void draw(VkCommandBuffer * commandBuffer, VkPipelineLayout * layout);
	
	// Updates the world and checks for win
	void update(double detlaTime);
	
	// Processes user input
	void processInput(int key, int action);

	// Win condition logic
	bool hasWon();

	// Toggles paused state
	void togglePaused();

	// Adds model to map
	void addModel(std::string modelName, DominusModel* model);

	// Model info functions
	uint32_t getModelVertexOffset(std::string modelName);
	uint32_t getModelIndexOffset(std::string modelName);
	uint32_t getModelVerticeCount(std::string modelName);
	uint32_t getModelIndexCount(std::string modelName);
};

