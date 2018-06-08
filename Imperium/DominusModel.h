#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "glm.hpp"
#include "Vertex.h"
#include <vector>
#include <string>

class DominusModel
{
public:
	// Unique model vertices
	std::vector<Vertex> vertices;

	// Model indices
	std::vector<uint32_t> indices;

	// Path to model file
	std::string filePath;

	DominusModel();
	DominusModel(const std::string file);
	~DominusModel();

	// Loads vertices and indices to vector
	virtual bool loadFromFile();

	// Overload << operator
	friend std::ostream& operator<<(std::ostream& os, DominusModel& dModel);
private:
};