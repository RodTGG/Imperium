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

std::string DominusTools::queueFamilyFlagToString(const VkQueueFamilyProperties queueFamily)
{
	std::string result;

	if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		result += "GRAPHICS_BIT, ";

	if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
		result += "COMPUTE_BIT, ";

	if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
		result += "TRANSFER_BIT";

	return result;
}

char* DominusTools::presentModeToString(const VkPresentModeKHR presentMode)
{
	switch (presentMode)
	{
	case VK_PRESENT_MODE_FIFO_KHR:
		return "FIFO_KHR";
		break;
	case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
		return "FIFO_RELAXED_KHR";
		break;
	case VK_PRESENT_MODE_MAILBOX_KHR:
		return "MAILBOX_KHR";
		break;
	case VK_PRESENT_MODE_IMMEDIATE_KHR:
		return "IMMEDIATE_KHR";
		break;
	default:
		return "Unknown present mode";
		break;
	}
}

std::string DominusTools::surfaceFormatToString(const VkSurfaceFormatKHR surfaceFormat)
{
	std::string result;

	switch (surfaceFormat.format)
	{
	case VK_FORMAT_B8G8R8A8_UNORM:
		result = "B8G8R8A8_UNORM";
		break;
	case VK_FORMAT_UNDEFINED:
		result = "No preference";
		break;
	default:
		result = "Unknown present mode format";
		break;
	}

	result += ", ";

	switch (surfaceFormat.colorSpace)
	{
	case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
		result += "SRGB_NONLINEAR_KHR";
		break;
	case VK_COLOR_SPACE_RANGE_SIZE_KHR:
		result += "RANGE_SIZE_KHR";
		break;
	case VK_COLOR_SPACE_MAX_ENUM_KHR:
		result += "MAX_ENUM_KHR";
	default:
		result += "unknown color space";
		break;
	}

	return result;
}
