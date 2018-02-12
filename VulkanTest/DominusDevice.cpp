#include "DominusDevice.h"
#include "DominusTools.h"
#include <set>
#include <iostream>

DominusDevice::DominusDevice()
{
	physicalDevice = VK_NULL_HANDLE;
	graphicsCommandPool = VK_NULL_HANDLE;
}

DominusDevice::DominusDevice(VkPhysicalDevice aPhysicalDevice)
{
	uint32_t queueFamilyCount = 0;
	uint32_t extensionCount = 0;

	physicalDevice = aPhysicalDevice;
	graphicsCommandPool = VK_NULL_HANDLE;

	vkGetPhysicalDeviceProperties(physicalDevice, &properties);

	vkGetPhysicalDeviceFeatures(physicalDevice, &features);

	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	queueFamilyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

	vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

	if (extensionCount > 0)
	{
		std::vector<VkExtensionProperties> extensions(extensionCount);
		if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data())
			!= VK_SUCCESS) {
			throw std::runtime_error("Failed to enumerate extensions!");
		}

		deviceExtensions.reserve(extensionCount);

		for (auto ext : extensions)
		{
			deviceExtensions.push_back(ext.extensionName);
		}
	}
}

DominusDevice::~DominusDevice()
{
}

VkResult DominusDevice::createLogicalDevice(VkPhysicalDeviceFeatures aEnableFeatures, std::vector<const char*> enableExtensionsName, VkQueueFlags queue)
{
	//#ifdef _DEBUG
	//	const bool enableValidationLayers = true;
	//#else
	//	const bool enableValidationLayers = false;
	//#endif //  NDEBUG

	VkResult result;
	float queuePriority = 1.0f;

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int32_t> uniqueQueueFamilies = { queueFamilyIndices.graphics, queueFamilyIndices.present };

	for (int32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &aEnableFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(enableExtensionsName.size());
	createInfo.ppEnabledExtensionNames = enableExtensionsName.data();

	//	if (enableValidationLayers)
	{
		char* validation = "VK_LAYER_LUNARG_standard_validation";
		createInfo.enabledLayerCount = static_cast<uint32_t>(1);
		createInfo.ppEnabledLayerNames = &validation;
	}
	/*else
		createInfo.enabledLayerCount = 0;*/

	result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice);
	enabledFeatures = aEnableFeatures;

	return result;
}

//DominusBuffer DominusDevice::createBuffer(DominusBuffer & buffer)
//{
//	std::cout << "Creating buffer:" << std::endl;
//	std::cout << "\tSize: " << buffer.size << " bytes" << std::endl;
//	std::cout << "\tUsage: " << buffer.usageFlags << std::endl;
//
//	VkBufferCreateInfo bufferInfo = {};
//	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//	bufferInfo.size = buffer.size;
//	bufferInfo.usage = buffer.usageFlags;
//	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//	if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer.buffer) != VK_SUCCESS)
//		throw std::runtime_error("Failed to create buffer");
//
//	VkMemoryRequirements memRequirements;
//	vkGetBufferMemoryRequirements(logicalDevice, buffer.buffer, &memRequirements);
//
//	VkMemoryAllocateInfo allocInfo = {};
//	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//	allocInfo.allocationSize = memRequirements.size;
//	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, buffer.memoryPropertyFlags);
//
//	if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &buffer.bufferMemory) != VK_SUCCESS)
//		throw std::runtime_error("Failed to allocate memory!");
//
//	buffer.setDescriptor();
//	buffer.bind();
//
//	return buffer;
//}

VkResult DominusDevice::createBuffer(DominusBuffer& buffer, const VkDeviceSize size, const VkBufferUsageFlags usageFlags, const VkMemoryPropertyFlags memoryPropertyFlags)
{
	std::cout << "Creating buffer" << std::endl;
	std::cout << "\tSize: " << size << " bytes" << std::endl;
	std::cout << "\tUsage: " << usageFlags << std::endl;

	//buffer = DominusBuffer(logicalDevice, size, usageFlags, memoryPropertyFlags);
	buffer.device = logicalDevice;
	buffer.bufferSize = size;
	buffer.usageFlags = usageFlags;
	buffer.memoryPropertyFlags = memoryPropertyFlags;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usageFlags;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer.buffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to create buffer");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(logicalDevice, buffer.buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memoryPropertyFlags);

	if (vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &buffer.bufferMemory) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate memory!");

	buffer.setDescriptor();
	buffer.bind();

	return VK_SUCCESS;
}

VkResult DominusDevice::copyBuffer(DominusBuffer & aSource, DominusBuffer & aDestination, const VkBufferCopy& copyRegion, const VkQueue& queue)
{
	VkCommandBuffer cmdBuffer;

	std::cout << "Copying buffer" << std::endl;

	createCommandBuffer(cmdBuffer);
	vkCmdCopyBuffer(cmdBuffer, aSource.buffer, aDestination.buffer, 1, &copyRegion);
	submitCommandBuffer(cmdBuffer, queue);

	return VK_SUCCESS;
}

void DominusDevice::createCommandBuffer(VkCommandBuffer& commandBuffer, const VkCommandBufferLevel & level)
{
	std::cout << "Creating command buffer" << std::endl;

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = level;
	allocInfo.commandPool = graphicsCommandPool;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to create command buffer");

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("Failed to begin command buffer");
}

VkResult DominusDevice::submitCommandBuffer(VkCommandBuffer & commandBuffer, const VkQueue & queue)
{
	std::cout << "Submitting commands" << std::endl;

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("Failed to end command buffer");

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkFence cmdFence = createFence();

	if (vkQueueSubmit(queue, 1, &submitInfo, cmdFence) != VK_SUCCESS)
		throw std::runtime_error("Failed to submit to queue");

	if (vkWaitForFences(logicalDevice, 1, &cmdFence, VK_FALSE, UINT64_MAX) != VK_SUCCESS)
		throw std::runtime_error("Failed to wait for fences");

	vkDestroyFence(logicalDevice, cmdFence, nullptr);
	vkFreeCommandBuffers(logicalDevice, graphicsCommandPool, 1, &commandBuffer);

	return VK_SUCCESS;
}

VkFence DominusDevice::createFence(const VkFenceCreateFlags flags)
{
	VkFence result;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = flags;

	if (vkCreateFence(logicalDevice, &fenceInfo, nullptr, &result) != VK_SUCCESS)
		throw std::runtime_error("Failed to crate VkFence");

	return result;
}

void DominusDevice::querySwapChainSupport(const VkSurfaceKHR& surface)
{
	uint32_t formatCount = 0;
	uint32_t presentModeCount = 0;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

	if (formatCount > 0)
	{
		surfaceFormats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());
	}

	if (presentModeCount > 0)
	{
		surfacePresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, surfacePresentModes.data());
	}
}

int32_t DominusDevice::getQueueFamilyIndex(const VkQueueFlags& flags)
{
	for (size_t i = 0; i < queueFamilyProperties.size(); i++)
	{
		if (queueFamilyProperties[i].queueFlags & flags)
		{
			return (int32_t)i;
			break;
		}
	}

	std::cout << "Couldnt find queue family index for Queue flag bits " << flags << std::endl;

	return -1;
}

int32_t DominusDevice::getQueueFamilyIndexPresentation(const VkSurfaceKHR& surface)
{
	VkBool32 presentSupport;

	for (size_t i = 0; i < queueFamilyProperties.size(); i++)
	{
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, (int32_t)i, surface, &presentSupport);

		if (presentSupport)
		{
			return (int32_t)i;
			break;
		}
	}

	std::cout << "Couldnt find queue family index with presentation support" << std::endl;

	return -1;
}

bool DominusDevice::isExtensionSupported(const char * extensionName)
{
	return std::find(deviceExtensions.begin(), deviceExtensions.end(), extensionName) != deviceExtensions.end();
}

uint32_t DominusDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	throw std::runtime_error("Failed to find suitable memory type!");
}

DominusDevice::operator VkDevice()
{
	return logicalDevice;
}

std::ostream & operator<<(std::ostream & os, const DominusDevice & device)
{
	os << "Device details:" << std::endl;
	os << "Info" << std::endl;
	os << "\tId: " << device.properties.deviceID << std::endl;
	os << "\tName: " << device.properties.deviceName << std::endl;
	os << "\tVendor: " << device.properties.vendorID << std::endl;
	os << "\tDriver: " << device.properties.driverVersion << std::endl;
	os << "Supported extensions (" << device.deviceExtensions.size() << ")" << std::endl;

	for (auto extension : device.deviceExtensions)
		os << "\tName: " << extension << std::endl;

	os << "Supported queues ( " << device.queueFamilyProperties.size() << ")" << std::endl;

	for (auto queue : device.queueFamilyProperties)
	{
		os << "\tCount: " << queue.queueCount << std::endl;
		os << "\tFlags: " << DominusTools::queueFamilyFlagToString(queue) << std::endl;
	}

	os << "Supported surface capabilities" << std::endl;
	os << "\tMin image count: " << device.capabilities.minImageCount << std::endl;
	os << "\tMax image count: " << device.capabilities.maxImageCount << std::endl;
	os << "\tWidth: " << device.capabilities.currentExtent.width << std::endl;
	os << "\tHeight: " << device.capabilities.currentExtent.height << std::endl;

	os << "Supported formats (" << device.surfaceFormats.size() << ") [Format, colorSpace]" << std::endl;

	for (auto format : device.surfaceFormats)
		os << "\t" << DominusTools::surfaceFormatToString(format) << std::endl;

	os << "Surface present modes (" << device.surfacePresentModes.size() << ")" << std::endl;

	for (auto presentMode : device.surfacePresentModes)

		os << "\t" << DominusTools::presentModeToString(presentMode) << std::endl;


	return os;
}
