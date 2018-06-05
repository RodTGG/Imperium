#pragma once
#include "DominusModel.h"
#include <vulkan\vulkan.h>
#include <glm.hpp>

class DominusTools
{
public:
	DominusTools();
	~DominusTools();

	//static std::vector<char> readFile(const std::string& filePath);

	// Load binary SPV file and return created VkShaderModule
	static VkShaderModule loadShader(const VkDevice& device, const std::string& filePath);

	// Returns custom name for queue family property queue flags
	static std::string queueFamilyFlagToString(const VkQueueFamilyProperties& queueFamily);

	// Returns custom name for present mode object
	static char* presentModeToString(const VkPresentModeKHR& presentMode);
	
	// Returns custom name for surface format object
	static std::string vkSurfaceFormatToString(const VkSurfaceFormatKHR& surfaceFormat);

	// Returns string containing VkError type
	static std::string vkResultToString(const VkResult& vkResult);

	// Returns string filled with glm::mat4 data
	static std::string mat4ToString(glm::mat4 mat4);

	// Returns a string to represent the object type
	static std::string DebugObjectToString(VkObjectType object);
private:
};