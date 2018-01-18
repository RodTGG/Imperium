#pragma once
#include "DominusBuffer.h"
#include <vulkan\vulkan.hpp>
#include <vector>

class DominusDevice
{
public:
	// Logical device interface
	VkDevice logicalDevice;

	// Physical device interface
	VkPhysicalDevice physicalDevice;

	// Physical device properties
	VkPhysicalDeviceProperties properties;

	// All physical device supported features
	VkPhysicalDeviceFeatures features;

	// Features that will be enabled
	VkPhysicalDeviceFeatures enabledFeatures;

	// Physical device memory properties
	VkPhysicalDeviceMemoryProperties memoryProperties;

	// List of supported queue families properties
	std::vector<VkQueueFamilyProperties> queueFamilyProperties;

	// Command pool for graphics family
	VkCommandPool graphicsCommandPool;

	// Device surface capabilities
	VkSurfaceCapabilitiesKHR capabilities;

	// Extensions to enable
	std::vector<std::string> deviceExtensions;

	// Supported device surface formats
	std::vector<VkSurfaceFormatKHR> surfaceFormats;
	
	// Supported device present modes
	std::vector<VkPresentModeKHR> surfacePresentModes;

	struct 
	{
		uint32_t graphics;
		uint32_t present;
	} queueFamilyIndices;

	DominusDevice();
	DominusDevice(VkPhysicalDevice physicalDevice);
	~DominusDevice();

	// Attempt to create logical device
	VkResult createLogicalDevice(const VkPhysicalDeviceFeatures enableFeatures, const std::vector<const char*> enableExtensionsName, const VkQueueFlags queue);

	VkResult createBuffer(DominusBuffer& buffer);

	VkResult createBuffer(DominusBuffer& buffer, const VkDeviceSize size, const VkBufferUsageFlags usageFlags, const VkMemoryPropertyFlags memoryPropertyFlags);

	// Query for swap chain support
	void querySwapChainSupport(const VkSurfaceKHR& surface);

	// Returns first queue family that supports queue flags
	int32_t getQueueFamilyIndex(const VkQueueFlags& flags);

	// Returns first queue that supports surface presentation
	int32_t getQueueFamilyIndexPresentation(const VkSurfaceKHR& surface);

	// Check if extension is supported by the device
	bool isExtensionSupported(const char* extensionName);

	// Check if memory type provided is supported by device
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	// Typecast to VkDevice
	operator VkDevice();

	// Overload << Operator
	friend std::ostream& operator<<(std::ostream& os, const DominusDevice& device);
private:
};

