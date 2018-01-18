#include "DominusBuffer.h"
#include "DominusTools.h"
#include <iostream>

DominusBuffer::DominusBuffer()
{
}

DominusBuffer::DominusBuffer(const VkDevice & aDevice, const VkDeviceSize aSize, const VkBufferUsageFlags aUsageFlags, const VkMemoryPropertyFlags aMemoryPropertyFlags)
{
	device = aDevice;
	size = aSize;
	usageFlags = aUsageFlags;
	memoryPropertyFlags = aMemoryPropertyFlags;
}

DominusBuffer::~DominusBuffer()
{
}

void DominusBuffer::bind()
{
	std::cout << "Binding buffer to device" << std::endl;

	if (vkBindBufferMemory(device, buffer, bufferMemory, offset) != VK_SUCCESS)
		throw std::runtime_error("Failed to bind buffer!");
}

void DominusBuffer::map()
{
	std::cout << "Mapping buffer memory to device" << std::endl;

	if (vkMapMemory(device, bufferMemory, offset, size, 0, &pData) != VK_SUCCESS)
		throw std::runtime_error("Failed to map memory!");
}

void DominusBuffer::setDescriptor(VkDeviceSize aSize, VkDeviceSize aOffset)
{
	descriptor.buffer = buffer;
	descriptor.offset = aOffset;
	descriptor.range = aSize;
}

void DominusBuffer::copyTo(void* aDest)
{
	std::cout << "Copying memory to destination: " <<  aDest << std::endl;

	memcpy(pData, aDest, (size_t)size);
}

void DominusBuffer::transfer(void * dest)
{
	std::cout << "Transfering data" << std::endl;

	map();
	copyTo(dest);
	unmapBuffer();
}

void DominusBuffer::unmapBuffer()
{
	std::cout << "Unmapping memory from device" << std::endl;

	if (pData)
	{
		vkUnmapMemory(device, bufferMemory);
		pData = nullptr;
	}
}

void DominusBuffer::destroy()
{
	std::cout << "Destroying buffer" << std::endl;

	if (buffer)
	{
		vkDestroyBuffer(device, buffer, nullptr);
		buffer = VK_NULL_HANDLE;
	}

	if (bufferMemory)
	{
		vkFreeMemory(device, bufferMemory, nullptr);
		bufferMemory = VK_NULL_HANDLE;
	}
}