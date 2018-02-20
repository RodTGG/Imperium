#include "DominusTools.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

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

std::string DominusTools::queueFamilyFlagToString(const VkQueueFamilyProperties& queueFamily)
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

char* DominusTools::presentModeToString(const VkPresentModeKHR& presentMode)
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

std::string DominusTools::vkSurfaceFormatToString(const VkSurfaceFormatKHR& surfaceFormat)
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

std::string DominusTools::vkResultToString(const VkResult & vkResult)
{
	std::string result;

	switch (vkResult)
	{
	case VK_SUCCESS:
		result = "SUCCESS";
		break;
	case VK_NOT_READY:
		result = "NOT READY";
		break;
	case VK_TIMEOUT:
		result = "TIMEOUT";
		break;
	case VK_EVENT_SET:
		result = "EVENT SET";
		break;
	case VK_EVENT_RESET:
		result = "EVENT RESET";
		break;
	case VK_INCOMPLETE:
		result = "EVENT INCOMPLETE";
		break;
	case VK_ERROR_OUT_OF_HOST_MEMORY:
		result = "OUT OF HOST MEMORY";
		break;
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		result = "OUT OF DEVICE MEMORY";
		break;
	case VK_ERROR_INITIALIZATION_FAILED:
		result = "INITIALIZATION FAILED";
		break;
	case VK_ERROR_DEVICE_LOST:
		result = "DEVICE LOST";
		break;
	case VK_ERROR_MEMORY_MAP_FAILED:
		result = "MEMORY MAP FAILED";
		break;
	case VK_ERROR_LAYER_NOT_PRESENT:
		result = "LAYER NOT PRESENT";
		break;
	case VK_ERROR_EXTENSION_NOT_PRESENT:
		result = "EXTENSION NOT PRESENT";
		break;
	case VK_ERROR_FEATURE_NOT_PRESENT:
		result = "FEATURE NOT PRESENT";
		break;
	case VK_ERROR_INCOMPATIBLE_DRIVER:
		result = "INCOMPATIBLE DRIVER";
		break;
	case VK_ERROR_TOO_MANY_OBJECTS:
		result = "TOO MANY OBJECTS";
		break;
	case VK_ERROR_FORMAT_NOT_SUPPORTED:
		result = "FORMAT NOT SUPPORTED";
		break;
	case VK_ERROR_FRAGMENTED_POOL:
		break;
	case VK_ERROR_SURFACE_LOST_KHR:
		result = "SURFACE LOST KHR";
		break;
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
		break;
	case VK_SUBOPTIMAL_KHR:
		result = "SUBOPTIMAL KHR";
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		result = "OUT OF DATE KHR";
		break;
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
		break;
	case VK_ERROR_VALIDATION_FAILED_EXT:
		break;
	case VK_ERROR_INVALID_SHADER_NV:
		break;
	case VK_ERROR_OUT_OF_POOL_MEMORY_KHR:
		break;
	case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR:
		break;
	case VK_ERROR_NOT_PERMITTED_EXT:
		break;
	case VK_RESULT_RANGE_SIZE:
		break;
	case VK_RESULT_MAX_ENUM:
		break;
	default:
		break;
	}

	return result;
}