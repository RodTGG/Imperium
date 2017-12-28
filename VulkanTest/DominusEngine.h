#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define STB_IMAGE_IMPLEMENTATION

#include <GLFW\glfw3.h>
#include <vector>
#include <array>
#include <string>

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>
#include <glm.hpp>

struct UniformBufferObject
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}

	bool operator==(const Vertex& other) const
	{
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
};


namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

class Dominus
{
public:
	Dominus();
	~Dominus();

	void run();
private:
	const uint32_t WIDTH = 1200;
	const uint32_t HEIGHT = 720;

	const std::string MODEL = "meshes/chalet.obj";
	const std::string TEXTURE = "textures/chalet.jpg";

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
	float delta;

	GLFWwindow* gWindow;
	VkInstance gInstance;
	VkDebugReportCallbackEXT gCallback;
	VkPhysicalDevice gPhysicalDevice;
	VkQueue gGraphicsQueue;
	VkQueue gPresentQueue;
	VkDevice gDevice;
	VkSurfaceKHR gSurface;
	VkSurfaceCapabilitiesKHR gCapabilities;
	UniformBufferObject ubo = {};

	VkSwapchainKHR gSwapChain;
	VkFormat gSwapChainFormat;
	VkExtent2D gSwapChainExtent;
	VkRenderPass gRenderPass;
	VkDescriptorSetLayout descriptionSetLayout;
	VkPipelineLayout gPipelineLayout;
	VkPipeline gGraphicsPipeline;
	VkPipeline gGraphicsPipeline2;
	VkCommandPool commandPool;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	VkBuffer uniformBuffer;
	VkDeviceMemory uniformBufferMemory;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;

	std::vector<VkSurfaceFormatKHR> gFormats;
	std::vector<VkPresentModeKHR> gPresentModes;
	std::vector<VkFramebuffer> gSwapChainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

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
	void createDepthResources();
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& memory);
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void createTextureImageView();
	void createImageViews();
	void createTextureSampler();
	void loadModel();

	void swapGraphicsPipeline(VkPipeline aPipeline);

	bool isDeviceSuitable(VkPhysicalDevice aDevice);
	bool checkValidationLayerSupport();
	bool checkDeviceExtensionSupport(VkPhysicalDevice aDevice);
	bool hasStencilComponent(VkFormat format);

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	std::vector<const char*> getRequiredExtensions();

	VkFormat findDepthFormat();
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkImageView  createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	VkCommandBuffer beginSingleTimeCommands();
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

	//const std::vector<Vertex> vertices = {
	//	// front
	//	{ { -0.5, -0.5, 0.5 },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
	//{ { 0.5, -0.5,  0.5 },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
	//{ { 0.5,  0.5,  0.5 },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
	//{ { -0.5,  0.5,  0.5 },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },

	//// back
	//{ { -0.5, -0.5, -0.5 },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
	//{ { -0.5, 0.5, -0.5 },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
	//{ { 0.5, 0.5, -0.5 },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
	//{ { 0.5, -0.5, -0.5 },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },

	//// top
	//{ { -0.5, 0.5, -0.5 },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
	//{ { -0.5, 0.5, 0.5 },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
	//{ { 0.5, 0.5, 0.5 },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
	//{ { 0.5, 0.5, -0.5 },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },

	//// bottom
	//{ { -0.5, -0.5, -0.5 },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
	//{ { 0.5, -0.5, -0.5 },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
	//{ { 0.5, -0.5, 0.5 },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
	//{ { -0.5, -0.5, 0.5 },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },

	//// right
	//{ { 0.5, -0.5, -0.5 },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
	//{ { 0.5, 0.5, -0.5 },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
	//{ { 0.5, 0.5, 0.5 },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
	//{ { 0.5, -0.5, 0.5 },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },

	//// left
	//{ { -0.5, -0.5, -0.5 },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
	//{ { -0.5, -0.5, 0.5 },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
	//{ { -0.5, 0.5, 0.5 },{ 0.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
	//{ { -0.5, 0.5, -0.5 },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },
	//};

	//const std::vector<uint16_t> indices = {
	//	0,  1,  2,		2,  3,  0,    // front
	//	4,  5,  6,      6,  7,  4,    // back
	//	8,  9,  10,     10,  11, 8,   // top
	//	12, 13, 14,     14, 15, 12,   // bottom
	//	16, 17, 18,     18, 19, 16,   // right
	//	20, 21, 22,     22, 23, 20,   // left
	//};
};