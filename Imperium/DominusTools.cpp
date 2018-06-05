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
		throw std::runtime_error("\nFailed to open file");
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

std::string DominusTools::mat4ToString(glm::mat4 mat4)
{
	std::string result;

	for (auto i = 0; i < 4; i++)
	{
		result += "\n\t\t[";
		for (auto j = 0; j < 4; j++)
		{
			result += std::to_string(mat4[i][j]);
			result += " ";
		}
		result += "]";
	}

	return result;
}

std::string DominusTools::DebugObjectToString(VkObjectType object)
{
	std::string result;

	switch (object)
	{
	case VK_OBJECT_TYPE_UNKNOWN:
		result = "Unkown";
		break;
	case VK_OBJECT_TYPE_INSTANCE:
		result = "Instance";
		break;
	case VK_OBJECT_TYPE_PHYSICAL_DEVICE:
		result = "Physical Device";
		break;
	case VK_OBJECT_TYPE_DEVICE:
		result = "Logical Device";
		break;
	case VK_OBJECT_TYPE_QUEUE:
		result = "Queue";
		break;
	case VK_OBJECT_TYPE_SEMAPHORE:
		result = "Semaphore";
		break;
	case VK_OBJECT_TYPE_COMMAND_BUFFER:
		result = "Command Buffer";
		break;
	case VK_OBJECT_TYPE_FENCE:
		result = "Fence";
		break;
	case VK_OBJECT_TYPE_DEVICE_MEMORY:
		result = "Device Memory";
		break;
	case VK_OBJECT_TYPE_BUFFER:
		result = "Buffer";
		break;
	case VK_OBJECT_TYPE_IMAGE:
		result = "Image";
		break;
	case VK_OBJECT_TYPE_EVENT:
		result = "EVENT";
		break;
	case VK_OBJECT_TYPE_QUERY_POOL:
		result = "Query Pool";
		break;
	case VK_OBJECT_TYPE_BUFFER_VIEW:
		result = "Buffer View";
		break;
	case VK_OBJECT_TYPE_IMAGE_VIEW:
		result = "Image View";
		break;
	case VK_OBJECT_TYPE_SHADER_MODULE:
		result = "Shader Module";
		break;
	case VK_OBJECT_TYPE_PIPELINE_CACHE:
		result = "Pipeline Cache";
		break;
	case VK_OBJECT_TYPE_PIPELINE_LAYOUT:
		result = "Pipeline Layout";
		break;
	case VK_OBJECT_TYPE_RENDER_PASS:
		result = "Render Pass";
		break;
	case VK_OBJECT_TYPE_PIPELINE:
		result = "Pipeline";
		break;
	case VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT:
		result = "Descriptor Set Layout";
		break;
	case VK_OBJECT_TYPE_SAMPLER:
		result = "Sampler";
		break;
	case VK_OBJECT_TYPE_DESCRIPTOR_POOL:
		result = "Descritor Pool";
		break;
	case VK_OBJECT_TYPE_DESCRIPTOR_SET:
		result = "Descriptor Set";
		break;
	case VK_OBJECT_TYPE_FRAMEBUFFER:
		result = "Framebuffer";
		break;
	case VK_OBJECT_TYPE_COMMAND_POOL:
		result = "Command Pool";
		break;
	case VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION:
		result = "Sampler YCBCR Conversion";
		break;
	case VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE:
		result = "Descriptor Update Template";
		break;
	case VK_OBJECT_TYPE_SURFACE_KHR:
		result = "Surface KHR";
		break;
	case VK_OBJECT_TYPE_SWAPCHAIN_KHR:
		result = "Swapchain";
		break;
	case VK_OBJECT_TYPE_DISPLAY_KHR:
		result = "Display KHR";
		break;
	case VK_OBJECT_TYPE_DISPLAY_MODE_KHR:
		result = "Display Mode KHR";
		break;
	case VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT:
		result = "Debug Report Callback EXT";
		break;
	case VK_OBJECT_TYPE_OBJECT_TABLE_NVX:
		result = "Object Table NVX";
		break;
	case VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NVX:
		result = "Indirect Commands Layout NVX";
		break;
	case VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT:
		result = "Debug Utils Messenger EXT";
		break;
	case VK_OBJECT_TYPE_VALIDATION_CACHE_EXT:
		result = "Validation Cache EXT";
		break;
	case VK_OBJECT_TYPE_RANGE_SIZE:
		result = "Range Size";
		break;
	case VK_OBJECT_TYPE_MAX_ENUM:
		result = "Max Enum";
		break;
	default:
		result = "Unkown";
		break;
	}

	return result;
}
