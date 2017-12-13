#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define STB_IMAGE_IMPLEMENTATION

#include <GLFW\glfw3.h>
#include <vector>
#include <array>

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
	VkRenderPass gRenderPass;
	VkDescriptorSetLayout descriptionSetLayout;
	VkPipelineLayout gPipelineLayout;
	VkPipeline gGraphicsPipeline;
	VkCommandPool commandPool;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	VkBuffer uniformBuffer;
	VkDeviceMemory uniformBufferMemory;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;

	std::vector<VkSurfaceFormatKHR> gFormats;
	std::vector<VkPresentModeKHR> gPresentModes;
	std::vector<VkFramebuffer> gSwapChainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers;

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
	void createRenderPass();
	void createFramebuffers();
	void createCommandPool();
	void createVertexBuffer();
	void createIndexBuffer();
	void createCommandBuffers();
	void createSempahores();
	void drawFrame();
	void recreateSwapChain();
	void cleanupSwapChain();
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer	, VkDeviceSize size);
	void createDescriptionSetLayout();
	void createUniformBuffer();
	void updateUniformBuffer();
	void createDescriptorPool();
	void createDescriptorSet();
	void createTextureImage();

	bool isDeviceSuitable(VkPhysicalDevice aDevice);
	bool checkValidationLayerSupport();
	bool checkDeviceExtensionSupport(VkPhysicalDevice aDevice);

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	std::vector<const char*> getRequiredExtensions();

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkShaderModule createShaderModule(const std::vector<char>& code);
	VkResult createDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback);

	static void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void onWindowResized(GLFWwindow* window, int width, int height);
	static std::vector<char> readFile(const std::string& fileName);
	static void glfwErrorCallback(int error, const char* description);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData);
};