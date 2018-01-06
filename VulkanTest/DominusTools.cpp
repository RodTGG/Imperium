#include "DominusTools.h"

DominusTools::DominusTools()
{
}

DominusTools::~DominusTools()
{
}

// Read file and return vector with binary contents
//std::vector<char> DominusTools::readFile(const std::string & filePath)
//{
//	std::cout << "Reading file: " << filePath << std::endl;
//	std::ifstream file(filePath, std::ios::ate | std::ios::binary);
//
//	if (!file.is_open())
//	{
//		throw std::runtime_error("Failed to open file");
//	}
//
//	size_t fileSize = (size_t)file.tellg();
//	std::cout << " (" << fileSize << " bytes)" << std::endl;
//	std::vector<char> buffer(fileSize);
//	file.seekg(0);
//	file.read(buffer.data(), fileSize);
//	file.close();
//
//	return buffer;
//}

bool DominusTools::hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkShaderModule DominusTools::loadShader(const VkDevice & device, const std::string & filePath)
{
	VkShaderModule shaderModule;
	char* codeBuffer;

	std::cout << "Reading file: " << filePath;
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file");
	}

	size_t fileSize = (size_t)file.tellg();
	std::cout << " (" << fileSize << " bytes)" << std::endl;
	codeBuffer = new char[fileSize];
	file.seekg(0);
	file.read(codeBuffer, fileSize);
	file.close();

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = fileSize;
	createInfo.pCode = (uint32_t*)codeBuffer;

	std::cout << "Creating shader module" << std::endl;

	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		throw std::runtime_error("Failed to create shader module!");

	delete[] codeBuffer;

	return shaderModule;
}

uint32_t DominusTools::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	std::cout << "Finding memory type: " << typeFilter << std::endl;

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	throw std::runtime_error("Failed to find suitable memory type!");
}