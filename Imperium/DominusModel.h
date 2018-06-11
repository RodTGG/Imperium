#pragma once
#include "Vertex.h"
#include <vector>
#include <string>

class DominusModel
{
public:
	uint32_t vCount;
	uint32_t iCount;
	uint32_t vOffset;
	uint32_t iOffset;

	// Unique model vertices
	std::vector<Vertex> vertices;

	// Model indices
	std::vector<uint32_t> indices;

	// Path to model file
	std::string filePath;

	DominusModel();
	DominusModel(const std::string file);
	~DominusModel();

	void cleanVectors();

	// Loads vertices and indices to vector
	bool loadFromFile();

	// Overload << operator
	friend std::ostream& operator<<(std::ostream& os, DominusModel& dModel);
private:
};