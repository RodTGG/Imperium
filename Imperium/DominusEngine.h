#pragma once
#define GLFW_INCLUDE_VULKAN
#define STB_IMAGE_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL

#include "DominusDevice.h"
#include "DominusBuffer.h"
#include "DominusCamera.h"
#include "DominusTools.h"
#include "World.h"
#include "Vertex.h"

#include <vector>
#include <array>
#include <string>
#include <unordered_map>

#include <vulkan\vulkan.hpp>
#include <GLFW\glfw3.h>
#include <glm/mat4x4.hpp>

class DominusEngine
{
public:
	DominusEngine();
	~DominusEngine();

	void run();
private:
	enum pipelineModes
	{
		SOLID,
		LINE,
		POINT
	};	

	struct MVPBuffer
	{
		//glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	} mvp;

#ifdef _DEBUG
	const bool enableValidationLayers = true;
#else
	const bool enableValidationLayers = false;
#endif //  NDEBUG

	const uint32_t WIDTH = 1200;
	const uint32_t HEIGHT = 720;
	const std::string TEXTURE = "textures/default.jpg";
	const int MAX_FRAMES_IN_FLIGHT = 2;

	bool firstMouse = true;

	int currentPipeline = pipelineModes::LINE;

	double lastX;
	double lastY;
	double deltaTime;

	size_t currentFrame = 0;	

	GLFWwindow* gWindow;
	VkInstance gInstance;

	VkDebugUtilsMessengerEXT gDebugUtilsCallback;
	VkDebugReportCallbackEXT gDebugReportCallback;

	VkQueue gGraphicsQueue;
	VkQueue gPresentQueue;
	VkSurfaceKHR gSurface;

	VkSwapchainKHR gSwapChain;
	VkFormat gSwapChainFormat;
	VkExtent2D gSwapChainExtent;
	VkRenderPass gRenderPass;
	VkDescriptorSetLayout descriptionSetLayout;
	VkPipelineLayout gPipelineLayout;

	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	DominusBuffer vertexBuffer;
	DominusBuffer indexBuffer;
	DominusBuffer mvpBuffer;
	DominusCamera camera;
	DominusDevice gDevice;
	World world;

	std::unordered_map<int, VkPipeline> pipelines;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<Vertex> sceneVertices;
	std::vector<uint32_t> sceneIndices;
	std::vector<const char*> getRequiredExtensions();
	std::vector<VkFramebuffer> gSwapChainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	const std::vector<const char*> validationLayers = { "VK_LAYER_LUNARG_standard_validation" };
	const std::vector<const char*> requiredExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	void initVulkan();
	void initWindow();
	void gameLoop();
	void update(double deltaTime);
	void cleanUp();
	void createVKInstance();
	void setupDebugCallback();
	void pickPyshicalDevice();
	void createLogicalDevice();
	void createSurface();
	void createSwapChain();
	void createGraphicsPipeline();
	void createRenderPass();
	void createFramebuffers();
	void createCommandPool();
	void loadModels();
	void createVertexBuffer();
	void createIndexBuffer();
	void createCommandBuffers();
	void createDescriptorPool();
	void createDescriptorSet();
	void createSyncObjects();
	void recreateSwapChain();
	void cleanupSwapChain();
	void createDescriptionSetLayout();
	void createUniformBuffer();
	void createTextureImage();
	void createDepthResources();
	void createTextureImageView();
	void createImageViews();
	void createTextureSampler();
	void drawFrame();
	void updateCommandBuffers();

	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& memory);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	bool hasStencilComponent(VkFormat format);
	bool isDeviceSuitable(DominusDevice device);
	bool checkValidationLayerSupport();

	VkFormat findDepthFormat();
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	
	VkResult createDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback);
	VkResult createDebugUtilsCallbackEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback);
	void destroyDebugUtilsCallbackEXT(VkInstance aInstance, VkDebugUtilsMessengerEXT aCallback, const VkAllocationCallbacks* aAllocator);
	void destroyDebugReportCallbackEXT(VkInstance aInstance, VkDebugReportCallbackEXT aCallback, const VkAllocationCallbacks* aAllocator);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData);
	
	static void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void onMousePositionCallback(GLFWwindow* window, double posX, double posY);
	static void onWindowResized(GLFWwindow* window, int width, int height);
	static void glfwErrorCallback(int error, const char* description);
};