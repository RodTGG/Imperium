#pragma once
#include <vulkan\vulkan.h>

class DominusBuffer
{
public:
	VkDevice device;
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
	VkDeviceSize size = 0;
	VkDeviceSize offset = 0;
	VkDescriptorBufferInfo descriptor;
	VkBufferUsageFlags usageFlags;
	VkMemoryPropertyFlags memoryPropertyFlags;
	void* pData = nullptr;

	DominusBuffer();
	DominusBuffer(const VkDevice& device, const VkDeviceSize size, const VkBufferUsageFlags usageFlags, const VkMemoryPropertyFlags memoryPropertyFlags);
	~DominusBuffer();

	// Create VkBuffer find memory requirements allocate buffer memory
	// and bind buffer to device
	void create(const VkPhysicalDevice& physicalDevice);
	
	// Bind buffer to physical device
	void bind();

	// Map buffer and buffer memory to device
	void map();

	// Copy memory to destination
	void copyTo(void* dest);

	// Map buffer and memory to device, copy to destination and unmap
	void transfer(void* dest);

	// Unmap buffer memory 
	void unmapBuffer();

	// Destroy buffer and free buffer memory
	void destroy();
private:
};

