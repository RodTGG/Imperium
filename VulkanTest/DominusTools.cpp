#include "DominusTools.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

DominusTools::DominusTools()
{
}

DominusTools::~DominusTools()
{
}

// Read file and return vector with binary contents
//std::vector<char> DominusTools::readFile(const std::string & filePath)
//{
//	std::cout << "Reading file: " << filePath << std::endl;
//	std::ifstream file(filePath, std::ios::ate | std::ios::binary);
//
//	if (!file.is_open())
//	{
//		throw std::runtime_error("Failed to open file");
//	}
//
//	size_t fileSize = (size_t)file.tellg();
//	std::cout << " (" << fileSize << " bytes)" << std::endl;
//	std::vector<char> buffer(fileSize);
//	file.seekg(0);
//	file.read(buffer.data(), fileSize);
//	file.close();
//
//	return buffer;
//}

VkShaderModule DominusTools::loadShader(const VkDevice & device, const std::string & filePath)
{
	VkShaderModule shaderModule;
	char* codeBuffer;

	std::cout << "Reading file: " << filePath;
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file");
	}

	size_t fileSize = (size_t)file.tellg();
	std::cout << " (" << fileSize << " bytes)" << std::endl;
	codeBuffer = new char[fileSize];
	file.seekg(0);
	file.read(codeBuffer, fileSize);
	file.close();

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = fileSize;
	createInfo.pCode = (uint32_t*)codeBuffer;

	std::cout << "Creating shader module" << std::endl;

	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		throw std::runtime_error("Failed to create shader module!");

	delete[] codeBuffer;

	return shaderModule;
}

std::string DominusTools::queueFamilyFlagToString(const VkQueueFamilyProperties queueFamily)
{
	std::string result;

	if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		result += "GRAPHICS_BIT, ";

	if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
		result += "COMPUTE_BIT, ";

	if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
		result += "TRANSFER_BIT";

	return result;
}

char* DominusTools::presentModeToString(const VkPresentModeKHR presentMode)
{
	switch (presentMode)
	{
	case VK_PRESENT_MODE_FIFO_KHR:
		return "FIFO_KHR";
		break;
	case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
		return "FIFO_RELAXED_KHR";
		break;
	case VK_PRESENT_MODE_MAILBOX_KHR:
		return "MAILBOX_KHR";
		break;
	case VK_PRESENT_MODE_IMMEDIATE_KHR:
		return "IMMEDIATE_KHR";
		break;
	default:
		return "Unknown present mode";
		break;
	}
}

std::string DominusTools::surfaceFormatToString(const VkSurfaceFormatKHR surfaceFormat)
{
	std::string result;

	switch (surfaceFormat.format)
	{
	case VK_FORMAT_B8G8R8A8_UNORM:
		result = "B8G8R8A8_UNORM";
		break;
	case VK_FORMAT_UNDEFINED:
		result = "No preference";
		break;
	default:
		result = "Unknown present mode format";
		break;
	}

	result += ", ";

	switch (surfaceFormat.colorSpace)
	{
	case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
		result += "SRGB_NONLINEAR_KHR";
		break;
	case VK_COLOR_SPACE_RANGE_SIZE_KHR:
		result += "RANGE_SIZE_KHR";
		break;
	case VK_COLOR_SPACE_MAX_ENUM_KHR:
		result += "MAX_ENUM_KHR";
	default:
		result += "unknown color space";
		break;
	}

	return result;
}

//DominusModel DominusTools::loadModelFromFile(const std::string fPath)
//{
//	DominusModel result;
//
//	tinyobj::attrib_t attrib;
//	std::vector<tinyobj::shape_t> shapes;
//	std::vector<tinyobj::material_t> materials;
//	std::string err;
//
//	std::cout << "Loading model " << fPath << std::endl;
//
//	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fPath.c_str()))
//		throw std::runtime_error(err);
//
//	// Used to de-duplicate vertices that are in the same positions
//	std::unordered_map<Vertex, uint32_t> uniqueVertices = {};
//
//	for (const auto& shape : shapes)
//	{
//		for (const auto& index : shape.mesh.indices)
//		{
//			Vertex vertex = {};
//
//			vertex.pos = {
//				attrib.vertices[3 * index.vertex_index + 0],
//				attrib.vertices[3 * index.vertex_index + 1],
//				attrib.vertices[3 * index.vertex_index + 2]
//			};
//
//			vertex.texCoord = {
//				attrib.texcoords[2 * index.texcoord_index + 0],
//				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
//			};
//
//			vertex.color = {
//				1.0f, 1.0f, 1.0f
//			};
//
//			// Store unique vertices
//			if (uniqueVertices.count(vertex) == 0)
//			{
//				uniqueVertices[vertex] = static_cast<uint32_t>(result.vertices.size());
//				result.vertices.push_back(vertex);
//			}
//
//			// Simple pushback
//			// vertices.push_back(vertex);
//			
//			// Push back indices to unique vertex
//			result.indices.push_back(uniqueVertices[vertex]);
//
//			// Simple pushback
//			// indices.push_back(static_cast<uint32_t>(indices.size()));
//		}
//	}
//
//	std::cout << "Loaded " << uniqueVertices.size() << " vertices" << std::endl;
//
//	return result;
//}
