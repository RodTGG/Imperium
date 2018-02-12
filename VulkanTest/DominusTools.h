#pragma once
#include "DominusModel.h"
#include <vulkan\vulkan.h>

class DominusTools
{
public:
	DominusTools();
	~DominusTools();

	//static std::vector<char> readFile(const std::string& filePath);

	// Load binary SPV file and return created VkShaderModule
	static VkShaderModule loadShader(const VkDevice& device, const std::string& filePath);

	// Returns custom name for queue family property queue flags
	static std::string queueFamilyFlagToString(const VkQueueFamilyProperties queueFamily);

	// Return custom name for present mode object
	static char* presentModeToString(const VkPresentModeKHR presentMode);
	
	// Return custom name for surface format object
	static std::string surfaceFormatToString(const VkSurfaceFormatKHR surfaceFormat);

	//static DominusModel loadModelFromFile(const std::string fPath);
private:
};