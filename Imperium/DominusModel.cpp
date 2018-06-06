#include "DominusModel.h"
#include "DominusTools.h"
#include <gtc/matrix_transform.hpp>
#include <iostream>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

DominusModel::DominusModel()
{
}

DominusModel::DominusModel(DominusDevice& aDevice, const glm::vec3 aPosition, const glm::vec4 aColor)
{
	device = &aDevice;
	position = aPosition;
	rotation = glm::vec3(1.0f);
	scaling = glm::vec3(1.0f);
	UVScaling = glm::vec2(1.0f);
	modelMat = glm::mat4(1.0f);
	color = aColor;
}

DominusModel::~DominusModel()
{
}

void DominusModel::draw(VkCommandBuffer* buffer)
{
	vkCmdDrawIndexed(*buffer, static_cast<uint32_t>(indices.size()), 1, 0, vertexOffset, 0);
}

bool DominusModel::loadFromFile(const std::string fPath)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;

	std::cout << "Loading model " << fPath << std::endl;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fPath.c_str()))
		throw std::runtime_error(err);

	// Used to de-duplicate vertices that are in the same positions
	std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

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
				attrib.texcoords[2 * index.texcoord_index + 0] * UVScaling.x,
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1] * UVScaling.y
			};

			vertex.color = {
				1.0f, 1.0f, 1.0f
			};

			// Store unique vertices
			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			// Simple pushback
			// vertices.push_back(vertex);

			// Push back indices to unique vertex
			indices.push_back(uniqueVertices[vertex]);

			// Simple pushback
			// indices.push_back(static_cast<uint32_t>(indices.size()));
		}
	}

	return true;
}

void DominusModel::updateModelMatrix()
{
	glm::mat4 transMat = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 rotMat = glm::mat4(1.0f);
	rotMat = glm::rotate(rotMat, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotMat = glm::rotate(rotMat, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotMat = glm::rotate(rotMat, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scaling);
	modelMat = transMat * rotMat * scaleMat;
}

std::ostream & operator<<(std::ostream & os, DominusModel & dModel)
{
	os << "World info" << std::endl;
	os << "\tLocation x=" << dModel.position.x << " y=" << dModel.position.y << " z=" << dModel.position.z << std::endl;
	os << "\tRotation x=" << dModel.rotation.x << " y=" << dModel.rotation.y << " z=" << dModel.rotation.z << std::endl;
	os << "\tScaling x=" << dModel.scaling.x << " y=" << dModel.scaling.y << " z=" << dModel.scaling.z << std::endl;
	os << "\tUVScaling x=" << dModel.UVScaling.x << " y=" << dModel.UVScaling.y << std::endl;
	os << "Model Matrix " << std::endl;
	os << DominusTools::mat4ToString(dModel.modelMat) << std::endl;
	os << "Model info" << std::endl;
	os << "\tFile path: " << dModel.filePath << std::endl;
	os << "\tUnique Vertices: " << dModel.vertices.size() << std::endl;
	os << "\tIndices: " << dModel.indices.size();

	return os;
}
