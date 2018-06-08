#include "World.h"
#include <iostream>

World::World()
{
}

World::World(DominusDevice& aDevice)
{
	device = &aDevice;
	players.push_back(new Agent(1));
}

World::~World()
{
}

void World::loadWorld()
{
	/*DominusCharacter* p1 = new DominusCharacter("p1", "bop.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	p1->loadFromFile();
	p1->updateModelMatrix();
	p1->vertexOffset = 0;
	players.push_back(p1);
	sceneModels.push_back(p1);

	DominusCharacter* p2 = new DominusCharacter("p2", "invader.obj", glm::vec3(20.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
	p2->loadFromFile();
	p2->updateModelMatrix();
	p2->vertexOffset = p1->vertices.size();
	players.push_back(p2);	
	sceneModels.push_back(p2);*/

	for (auto p : players)
		p->prepare();
}

void World::update(double deltaTime)
{
	/*moveTime += deltaTime;

	if (moveTime >= 1.0)
	{
		std::cout << "Delta: " << deltaTime << std::endl;

		sceneModels[0]->color.x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		sceneModels[0]->color.y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		sceneModels[0]->color.z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		sceneModels[1]->position.x += 20.0 * deltaTime;
		sceneModels[1]->updateModelMatrix();

		moveTime = 0;
	}*/
}
