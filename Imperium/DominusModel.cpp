#include "DominusModel.h"
#include <iostream>
//#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

DominusModel::DominusModel()
{
}

DominusModel::DominusModel(const std::string file)
{
	filePath = file;
}

DominusModel::~DominusModel()
{
}

bool DominusModel::loadFromFile()
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	std::string path = "meshes\\" + filePath;

	std::cout << "Loading model " << path << std::endl;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str()))
		throw std::runtime_error(err);

	vertices.clear();
	indices.clear();

	// Used to de-duplicate vertices that are in the same positions
	//std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = {
				1.0f, 1.0f, 1.0f
			};

			// Store unique vertices
			/*if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}*/

			// Simple pushback
			vertices.push_back(vertex);

			// Push back indices to unique vertex
			// indices.push_back(uniqueVertices[vertex]);

			// Simple pushback
			 indices.push_back(static_cast<uint32_t>(indices.size()));
		}
	}

	return true;
}

std::ostream & operator<<(std::ostream & os, DominusModel & dModel)
{
	os << "Model info" << std::endl;
	os << "\tFile path: " << dModel.filePath << std::endl;
	os << "\tVertices: " << dModel.vertices.size() << std::endl;
	os << "\tIndices: " << dModel.indices.size();

	return os;
}
