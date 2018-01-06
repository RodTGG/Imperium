#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <vulkan\vulkan.h>

class DominusTools
{
public:
	DominusTools();
	~DominusTools();

	//static std::vector<char> readFile(const std::string& filePath);

	// Load binary SPV file and return created VkShaderModule
	static VkShaderModule loadShader(const VkDevice& device, const std::string& filePath);
	
	// Returns true if the format contains stencil component
	static bool hasStencilComponent(VkFormat format);

	// Returns index of memory type specified
	static uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
private:
};