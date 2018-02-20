#pragma once
#include "DominusBuffer.h"

#include <vector>
#include <vulkan\vulkan.hpp>

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
		int32_t graphics;
		int32_t present;
	} queueFamilyIndices;

	DominusDevice();
	DominusDevice(VkPhysicalDevice physicalDevice);
	~DominusDevice();

	// Attempt to create logical device
	VkResult createLogicalDevice(const VkPhysicalDeviceFeatures enableFeatures, const std::vector<const char*> enableExtensionsName, const VkQueueFlags queue);

	// Create default buffer
	//DominusBuffer createBuffer(DominusBuffer& buffer);

	// Create buffer with predefined settings
	VkResult createBuffer(DominusBuffer& buffer, const VkDeviceSize size, const VkBufferUsageFlags usageFlags, const VkMemoryPropertyFlags memoryPropertyFlags);

	// Copy buffer from source to destination using specified queue and VkBufferCopy
	VkResult copyBuffer(DominusBuffer& source, DominusBuffer& destination, VkDeviceSize size, VkQueue& queue);

	// Create VkFence
	// Fence flags default to unsignaled VK_NULL_HANDLE
	VkFence createFence(const VkFenceCreateFlags flags = 0);

	// Query for swap chain support
	void querySwapChainSupport(const VkSurfaceKHR& surface);

	// Returns first queue family that supports queue flags
	int32_t getQueueFamilyIndex(const VkQueueFlags& flags);

	// Returns first queue that supports surface presentation
	int32_t getQueueFamilyIndexPresentation(const VkSurfaceKHR& surface);

	// Check if memory type provided is supported by device
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	// Check if extension is supported by the device
	bool isExtensionSupported(const char* extensionName);

	// Typecast to VkDevice
	operator VkDevice();

	// Overload << Operator
	friend std::ostream& operator<<(std::ostream& os, const DominusDevice& device);

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueue queue);
private:
};

