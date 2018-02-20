#pragma once
#include <vulkan\vulkan.hpp>

class DominusBuffer
{
public:
	// TODO Use pointer to device instead of local copy

	// Logical VkDevice
	VkDevice device;

	// VkBuffer
	VkBuffer buffer = VK_NULL_HANDLE;

	// VkDeviceMemory pointer to buffer memory
	VkDeviceMemory bufferMemory = VK_NULL_HANDLE;

	// Buffer descriptor info
	VkDescriptorBufferInfo descriptor;

	// Buffer creation size
	VkDeviceSize bufferSize = 0;
	
	// Buffer creation usage flags
	VkBufferUsageFlags usageFlags;
	
	// Buffer creation memory property flags
	VkMemoryPropertyFlags memoryPropertyFlags;
	
	// Generic pointer to data
	void* pData = nullptr;

	DominusBuffer();
	DominusBuffer(const VkDevice& device, const VkDeviceSize size, const VkBufferUsageFlags usageFlags, const VkMemoryPropertyFlags memoryPropertyFlags);
	~DominusBuffer();
	
	// Bind buffer to physical device
	void bind(VkDeviceSize offset = 0);

	// Map buffer and buffer memory to device
	void map();

	// Sets up descriptor
	void setDescriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

	// Copy memory to destination
	void copyTo(void* src);

	// Map buffer and memory to device, copy to destination and unmap
	void transfer(void* src);

	void flush();

	// Unmap buffer memory 
	void unmap();

	// Destroy buffer and free buffer memory
	void destroy();
private:
};

