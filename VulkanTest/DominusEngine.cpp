#include "DominusEngine.h"
#include <iostream>
#include <set>
#include <algorithm>
#include <fstream>
#include <string>

Dominus::Dominus()
{
	gPhysicalDevice = VK_NULL_HANDLE;
}

Dominus::~Dominus()
{
}

void Dominus::initVulkan()
{
	createVKInstance();
	setupDebugCallback();
	createSurface();
	pickPyshicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createGraphicsPipeline();
}

void Dominus::createVKInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Test!";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateInstance(&createInfo, nullptr, &gInstance) != VK_SUCCESS)
		throw std::runtime_error("Failed to create instance");
}

void Dominus::querySwapChainSupport(VkPhysicalDevice aDevice)
{
	uint32_t formatCount = 0;
	uint32_t presentModeCount = 0;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(aDevice, gSurface, &gCapabilities);
	vkGetPhysicalDeviceSurfaceFormatsKHR(aDevice, gSurface, &formatCount, nullptr);

	if (formatCount != 0) {
		gFormats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(aDevice, gSurface, &formatCount, gFormats.data());
	}

	vkGetPhysicalDeviceSurfacePresentModesKHR(aDevice, gSurface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		gPresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(aDevice, gSurface, &presentModeCount, gPresentModes.data());
	}
}

void Dominus::createSwapChain()
{
	querySwapChainSupport(gPhysicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(gFormats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(gPresentModes);
	VkExtent2D extent = chooseSwapExtent(gCapabilities);

	uint32_t imageCount = gCapabilities.minImageCount + 1;

	if (gCapabilities.maxImageCount > 0 && imageCount > gCapabilities.maxImageCount)
		imageCount = gCapabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = gSurface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = { static_cast<uint32_t>(gQGraphicsFamily),  static_cast<uint32_t>(gQPresentFamily) };


	if (gQGraphicsFamily != gQPresentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = gCapabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(gDevice, &createInfo, nullptr, &gSwapChain) != VK_SUCCESS)
		throw std::runtime_error("Failed to create swap chain!");

	vkGetSwapchainImagesKHR(gDevice, gSwapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(gDevice, gSwapChain, &imageCount, swapChainImages.data());

	gSwapChainFormat = surfaceFormat.format;
	gSwapChainExtent = extent;
}

void Dominus::createImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());

	for (uint64_t i = 0; i < swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = gSwapChainFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(gDevice, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image views!");
		}
	}
}

void Dominus::createGraphicsPipeline()
{
	auto vertShaderCode = readFile("shaders/vert.spv");
	auto fragShaderCode = readFile("shaders/frag.spv");
}

void Dominus::setupDebugCallback()
{
	if (!enableValidationLayers) return;

	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;

	if (createDebugReportCallbackEXT(gInstance, &createInfo, nullptr, &gCallback) != VK_SUCCESS)
		throw std::runtime_error("Failed to set up debug callback!");
}

void Dominus::pickPyshicalDevice()
{
	uint32_t deviceCount = 0;

	vkEnumeratePhysicalDevices(gInstance, &deviceCount, nullptr);

	if (deviceCount == 0)
		throw std::runtime_error("Failed to find GPU's with Vulkan support");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(gInstance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device)) {
			gPhysicalDevice = device;
			break;
		}
	}

	if (gPhysicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("Failed to find a suitable GPU");
}

void Dominus::createLogicalDevice()
{
	float queuePriority = 1.0f;

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = { gQGraphicsFamily, gQPresentFamily };

	for (int queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(gPhysicalDevice, &createInfo, nullptr, &gDevice) != VK_SUCCESS)
		throw std::runtime_error("Failed to create logical device!");

	vkGetDeviceQueue(gDevice, gQGraphicsFamily, 0, &gGraphicsQueue);
	vkGetDeviceQueue(gDevice, gQPresentFamily, 0, &gPresentQueue);
}

void Dominus::createSurface()
{
	if (glfwCreateWindowSurface(gInstance, gWindow, nullptr, &gSurface) != VK_SUCCESS)
		throw std::runtime_error("Failed to create window surface!");
}

void Dominus::findQueueFamilies(VkPhysicalDevice aDevice)
{
	uint32_t queueFamilyCount = 0;

	vkGetPhysicalDeviceQueueFamilyProperties(aDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(aDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			gQGraphicsFamily = i;

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(aDevice, i, gSurface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport)
			gQPresentFamily = i;

		if (gQGraphicsFamily >= 0 && gQPresentFamily >= 0)
			break;

		i++;
	}
}

void Dominus::initWindow()
{
	int major, minor, revision;

	if (!glfwInit())
		throw std::runtime_error("Failed to initialize GLFW");

	glfwGetVersion(&major, &minor, &revision);
	std::cout << "Running GLFW (" << major << "," << minor << "," << revision << ")" << std::endl;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	gWindow = glfwCreateWindow(WIDTH, HEIGHT, "VulkanTestWindow", nullptr, nullptr);
}

void Dominus::run()
{
	initWindow();
	initVulkan();
	gameLoop();
	cleanUp();
}

void Dominus::gameLoop()
{
	while (!glfwWindowShouldClose(gWindow)) {
		glfwPollEvents();
	}
}

void Dominus::cleanUp()
{
	for (uint64_t i = 0; i < swapChainImageViews.size(); i++) {
		vkDestroyImageView(gDevice, swapChainImageViews[i], nullptr);
	}


	vkDestroySwapchainKHR(gDevice, gSwapChain, nullptr);
	vkDestroyDevice(gDevice, nullptr);
	destroyDebugReportCallbackEXT(gInstance, gCallback, nullptr);
	vkDestroySurfaceKHR(gInstance, gSurface, nullptr);
	vkDestroyInstance(gInstance, nullptr);
	glfwDestroyWindow(gWindow);
	glfwTerminate();
}

void Dominus::destroyDebugReportCallbackEXT(VkInstance aInstance, VkDebugReportCallbackEXT aCallback, const VkAllocationCallbacks * aAllocator)
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(aInstance, "vkDestroyDebugReportCallbackEXT");

	if (func != nullptr)
		func(aInstance, aCallback, aAllocator);
}

bool Dominus::isDeviceSuitable(VkPhysicalDevice aDevice)
{
	findQueueFamilies(aDevice);

	if (gQGraphicsFamily == -1 && gQPresentFamily == -1)
		return false;

	if (!checkDeviceExtensionSupport(aDevice))
		return false;

	querySwapChainSupport(aDevice);

	if (gFormats.empty() && gPresentModes.empty())
		return false;

	return true;
}

bool Dominus::checkValidationLayerSupport()
{
	unsigned int layerCount;

	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	std::cout << "Validation Layers: " << layerCount << std::endl;

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

bool Dominus::checkDeviceExtensionSupport(VkPhysicalDevice aDevice)
{
	uint32_t extensionCount = 0;

	vkEnumerateDeviceExtensionProperties(aDevice, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);

	vkEnumerateDeviceExtensionProperties(aDevice, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

std::vector<const char*> Dominus::getRequiredExtensions()
{
	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;

	std::vector<const char*> extensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (unsigned int i = 0; i < glfwExtensionCount; i++)
	{
		extensions.push_back(glfwExtensions[i]);
	}

	if (enableValidationLayers)
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	std::cout << "GLFW Number of extensions: " << glfwExtensionCount << std::endl;

	return extensions;
}

std::vector<char> Dominus::readFile(const std::string & fileName)
{
	std::cout << "Loading: " << fileName;
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);

	if (!file.is_open()) 
	{
		throw std::runtime_error("Failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::cout << "(" << fileSize << " bytes)" << std::endl;
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}



VkSurfaceFormatKHR Dominus::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

	for (const auto& format : availableFormats)
	{
		if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return format;

	}

	return availableFormats[0];
}

VkPresentModeKHR Dominus::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes)
{
	VkPresentModeKHR bestPresentMode = VK_PRESENT_MODE_FIFO_KHR;

	std::cout << "Selecting best swap present mode (" << availableModes.size() << ")" << std::endl;

	for (const auto& mode : availableModes)
	{
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			std::cout << "Present Mode: VK_PRESENT_MODE_MAILBOX_KHR" << std::endl;
			return mode;
		}
		else if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			std::cout << "Present Mode: VK_PRESENT_MODE_IMMEDIATE_KHR" << std::endl;
			bestPresentMode = mode;
		}
	}

	std::cout << "Present Mode: VK_PRESENT_MODE_FIFO_KHR" << std::endl;

	return bestPresentMode;
}

VkExtent2D Dominus::chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		return capabilities.currentExtent;
	else {
		VkExtent2D actualExtent = { WIDTH, HEIGHT };

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

VkResult Dominus::createDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");

	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pCallback);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Dominus::debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t locatio, int32_t code, const char * layerPrefix, const char * msg, void * userData)
{
	std::cerr << "validation layer: " << msg << std::endl;

	return VK_FALSE;
}