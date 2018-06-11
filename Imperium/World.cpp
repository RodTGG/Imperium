#include "World.h"
#include <iostream>

World::World()
{
}

World::World(DominusDevice& aDevice)
{
	device = &aDevice;
}

World::~World()
{
	for (auto i : players)
		delete i;
}

void World::loadWorld()
{
	addModel("invader", new DominusModel("invader.obj"));
	addModel("ball", new DominusModel("bop.obj"));

	for (auto m : models) {
		m.second->loadFromFile();
	}

	players.push_back(new Agent(this, 1, glm::vec3(0.0f, 0.0f, 0.0f)));
	players.push_back(new Agent(this, 2, glm::vec3(80.0f, 0.0f, 0.0f)));

	for (auto p : players)
		p->updateModelMatrix();
}

void World::update(double deltaTime)
{
	for (auto p : players)
		p->update(deltaTime);

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
