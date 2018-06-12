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
	players.push_back(new Agent(this, 2, Agent::BARRACKS, glm::vec3(80.0f, 0.0f, 0.0f)));

	minerals.push_back(new Mineral(this, glm::vec3(-20.f, 10.f, 0.f), 300, 10));
	minerals.push_back(new Mineral(this, glm::vec3(100.f, -20.f, 0.f), 300, 10));

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

	for (auto i = 0; i < players.size(); i++)
		players[i]->update(deltaTime);

	// Testing
	/*moveTime += deltaTime;

	if (moveTime >= 2.0)
	{
		std::cout << "Delta: " << deltaTime << std::endl;

		players[0]->color.x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		players[0]->color.y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		players[0]->color.z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		players[1]->position.x += 20.0f * deltaTime;
		players[1]->updateModelMatrix();

		Agent* tmp = new Agent(this, 1, glm::vec3(xOffset, 0.0f, 0.0f));
		tmp->model = "ball";
		tmp->updateModelMatrix();
		players.push_back(tmp);

		xOffset += 20.0f;
		moveTime = 0;
	}*/
}

void World::processInput(int key, int action)
{
	//if key == glfw
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
		throw std::runtime_error("Model not found for: " + modelName + " while getting vertice count");

	return models[modelName]->vCount;
}

uint32_t World::getModelIndexCount(std::string modelName)
{
	if (models.count(modelName) == 0)
		throw std::runtime_error("Model not found for: " + modelName + " while getting index count");

	return models[modelName]->iCount;
}
