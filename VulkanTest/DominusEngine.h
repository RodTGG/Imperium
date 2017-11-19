#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

class Dominus
{
public:
	Dominus();
	~Dominus();

	void run();
private:
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

#ifdef _DEBUG
	const bool enableValidationLayers = true;
#else
	const bool enableValidationLayers = false;
#endif //  NDEBUG

	const std::vector<const char*> validationLayers = { "VK_LAYER_LUNARG_standard_validation" };
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;

	int gQGraphicsFamily = -1;
	int gQPresentFamily = -1;

	GLFWwindow* gWindow;

	VkInstance gInstance;
	VkDebugReportCallbackEXT gCallback;
	VkPhysicalDevice gPhysicalDevice;
	VkQueue gGraphicsQueue;
	VkQueue gPresentQueue;
	VkDevice gDevice;
	VkSurfaceKHR gSurface;
	VkSurfaceCapabilitiesKHR gCapabilities;

	VkSwapchainKHR gSwapChain;
	VkFormat gSwapChainFormat;
	VkExtent2D gSwapChainExtent;

	std::vector<VkSurfaceFormatKHR> gFormats;
	std::vector<VkPresentModeKHR> gPresentModes;

	void initVulkan();
	void initWindow();
	void cleanUp();
	void gameLoop();
	void createVKInstance();
	void setupDebugCallback();
	void pickPyshicalDevice();
	void createLogicalDevice();
	void createSurface();
	void findQueueFamilies(VkPhysicalDevice);
	void destroyDebugReportCallbackEXT(VkInstance aInstance, VkDebugReportCallbackEXT aCallback, const VkAllocationCallbacks* aAllocator);
	void querySwapChainSupport(VkPhysicalDevice);
	void createSwapChain();
	void createImageViews();
	void createGraphicsPipeline();

	bool isDeviceSuitable(VkPhysicalDevice aDevice);
	bool checkValidationLayerSupport();
	bool checkDeviceExtensionSupport(VkPhysicalDevice aDevice);

	std::vector<const char*> getRequiredExtensions();
	static std::vector<char> readFile(const std::string& fileName);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t locatio, int32_t code, const char* layerPrefix, const char* msg, void* userData);
	VkResult createDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback);
};

