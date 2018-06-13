#pragma once
#include "DominusObject.h"

class Agent : public DominusObject
{
public:
	enum MODES
	{
		DEFAULT,
		UNIT,
		BASE,
		BARRACKS,
		TOWER,
		BULLET,
	};

	enum STATES
	{
		MINING,
	};

	/*std::unordered_map<std::string, std::unordered_map<std::string, int>> actions = {
		{"mine", { {"resources", 20}, {"time", 5}}},
	};*/

	// Agent info
	int hp = 100;
	int damage = 10;
	uint32_t team;
	uint32_t resource = 100;
	float collisionRadius = 5;
	bool alive = true;
	MODES mode;
	STATES state;

	//Barracks
	uint32_t unitCost = 20;

	//Time Limits
	float spawnTimer = 2.f;
	float spawnElapsed = 0.f;
	float collectionTime = 3.f;
	float collectionElapsed = 0.f;

	// Physics and limits
	float decelSpeeds[3] = { 0.9f, 0.5f, 0.2f };
	float maxSpeed = 10.0f;
	float maxForce = 1000.0f;
	float mass = 1.0;

	// Wander details
	glm::vec2 wanderTarget = glm::vec2(1.f, 0.f);
	float wanderDist = 1.f;
	float wanderRadius = 1.f;
	float wanderJitter = 20.f;

	// Physics Vectors
	glm::vec2 vel;
	glm::vec2 force;
	glm::vec2 accel;

	Agent();
	Agent(World* world, const uint32_t team, MODES mode = DEFAULT, const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec4& color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), const std::string modelName = "invader");
	~Agent();

	void draw(VkCommandBuffer* commandBuffer, VkPipelineLayout* layout) override;
	void update(double deltaTime) override;
	void updatePhysics(float fDeltaTime);
	void checkCollision();

	float getSpeed();
	glm::vec2 calculate(float delta);

	// Basic AI functions
	glm::vec2 seek(glm::vec3 targetPos);
	glm::vec2 flee(glm::vec3 targetPos);
	glm::vec2 arrive(glm::vec3 targetPos, float decelSpeed);
	glm::vec2 pursuit(Agent& evader);
	glm::vec2 wander(float delta);
	glm::vec2 mineMineral();

	Agent* getClosestEnemyUnit();
	Agent* getEnemyType(MODES mode);
	Agent* getTeamType(MODES mode);

	void spawnUnit(float delta);

	// TODO: Look for easier and more accurate way, maybe in built GLM function
	// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/
	glm::quat rotationBetweenVectors(glm::vec3 start, glm::vec3 dest);

	// Random float generator
	float randomUniform(float min, float max);
};

