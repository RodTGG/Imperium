#include "World.h"
#include <GLFW/glfw3.h>
#include <iostream>

World::World()
{
}

World::~World()
{
	for (auto p : players)
		delete p;

	for (auto m : minerals)
		delete m;

	for (auto dm : models)
		delete dm.second;
}

void World::loadWorld()
{
	addModel("invader", new DominusModel("invader.obj"));
	addModel("base", new DominusModel("base.obj"));
	addModel("mineral", new DominusModel("mineral.obj"));
	addModel("circle", new DominusModel("circle.obj"));
	addModel("triangle", new DominusModel("triangle.obj"));

	for (auto m : models)
		m.second->loadFromFile();

	players.push_back(new Agent(this, 1, Agent::DEFAULT, glm::vec3(0.0f, 0.0f, 0.0f)));
	players.push_back(new Agent(this, 2, Agent::DEFAULT, glm::vec3(80.0f, 0.0f, 0.0f)));

	players.push_back(new Agent(this, 1, Agent::BARRACKS, glm::vec3(-20.0f, 0.0f, 0.0f)));
	players.push_back(new Agent(this, 2, Agent::BARRACKS, glm::vec3(100.0f, 0.0f, 0.0f)));

	minerals.push_back(new Mineral(this, glm::vec3(-40.f, 10.f, 0.f), 300, 10));
	minerals.push_back(new Mineral(this, glm::vec3(120.f, -20.f, 0.f), 300, 10));

	for (auto p : players)
		p->prepare();

	for (auto m : minerals)
		m->prepare();
}

void World::draw(VkCommandBuffer * commandBuffer, VkPipelineLayout * layout)
{
	for (auto p : players)
		p->draw(commandBuffer, layout);

	for (auto m : minerals)
		m->draw(commandBuffer, layout);
}

void World::update(double deltaTime)
{
	// Ranged based loop iterator crashing on vector change mid iteration
	//for (auto p : players)
	//	p->update(deltaTime);

	// Remove dead agents
	players.erase(std::remove_if(players.begin(), players.end(), [](const Agent* a) {return !a->alive; }), players.end());

	// Check if a player has won
	if (!hasWon())
		for (auto i = 0; i < players.size(); i++)
			players[i]->update(deltaTime);
}

void World::processInput(int key, int action)
{
	//if key == glfw
}

bool World::hasWon()
{
	if (std::find_if(players.begin(), players.end(), [](const Agent* a) {return a->team == 1; }) == players.end())
	{
		winner = 2;
		won = true;
		return true;
	}
	else if (std::find_if(players.begin(), players.end(), [](const Agent* a) {return a->team == 2; }) == players.end())
	{
		winner = 1;
		won = true;
		return true;
	}

	return false;
}

void World::addModel(std::string modelName, DominusModel* model)
{
	if (models.count(modelName) != 0) {
		std::cout << "Model already exists skipping";
		return;
	}

	models[modelName] = model;
}

uint32_t World::getModelVertexOffset(std::string modelName)
{
	if (models.count(modelName) == 0)
		throw std::runtime_error("Model offset not found for: " + modelName);

	return models[modelName]->vOffset;
}

uint32_t World::getModelIndexOffset(std::string modelName)
{
	if (models.count(modelName) == 0)
		throw std::runtime_error("Model offset not found for: " + modelName);

	return models[modelName]->iOffset;
}

uint32_t World::getModelVerticeCount(std::string modelName)
{
	if (models.count(modelName) == 0)
		throw std::runtime_error("Model not found for: " + modelName + " while getting vertices's count");

	return models[modelName]->vCount;
}

uint32_t World::getModelIndexCount(std::string modelName)
{
	if (models.count(modelName) == 0)
		throw std::runtime_error("Model not found for: " + modelName + " while getting index count");

	return models[modelName]->iCount;
}
