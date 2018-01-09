#pragma once
#include <vulkan\vulkan.hpp>
#include <vector>

class DominusDevice
{
public:
	// Logical device interface
	VkDevice logicalDevice;

	// Physical device interface
	VkPhysicalDevice physicalDevice;

	// All physical device supported features
	VkPhysicalDeviceFeatures features;

	// Features that will be enabled
	VkPhysicalDeviceFeatures enabledFeatures;

	// Physical device memory properties
	VkPhysicalDeviceMemoryProperties memoryProperties;

	// List of supported queue families properties
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;

	// Extensions to enable
	std::vector<const char*> deviceExtensions;

	// Command pool for graphics family
	VkCommandPool graphicsCommandPool;

	DominusDevice();
	~DominusDevice();

private:
};

