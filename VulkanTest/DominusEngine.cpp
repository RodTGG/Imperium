#include "DominusEngine.h"
#include "Vertex.h"
#include <iostream>
#include <set>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <stb_image.h>
#include <unordered_map>
//#include <iomanip>

DominusEngine::DominusEngine()
{
}

DominusEngine::~DominusEngine()
{
	for (auto p : sceneModels)
		delete p;

	cleanUp();
}

void DominusEngine::initVulkan()
{
	createVKInstance();
	setupDebugCallback();
	createSurface();
	pickPyshicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createDescriptionSetLayout();
	createGraphicsPipeline();
	createCommandPool();
	createDepthResources();
	createFramebuffers();
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
	loadModels();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffer();
	createDescriptorPool();
	createDescriptorSet();
	createCommandBuffers();
	createSempahores();
}

void DominusEngine::initWindow()
{
	int major, minor, revision;
	//int monitorCount;

	glfwSetErrorCallback(glfwErrorCallback);

	if (!glfwInit())
		throw std::runtime_error("Failed to initialize GLFW");

	//GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);

	glfwGetVersion(&major, &minor, &revision);
	std::cout << "Running GLFW (" << major << "," << minor << "," << revision << ")" << std::endl;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	/* Fullscreen
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	gWindow = glfwCreateWindow(mode->width, mode->height, "VulkanTestWindow", monitor, nullptr);*/

	gWindow = glfwCreateWindow(WIDTH, HEIGHT, "VulkanTestWindow", nullptr, nullptr);
	glfwSetWindowSizeLimits(gWindow, 100, 100, GLFW_DONT_CARE, GLFW_DONT_CARE);

	// Set window pointer to engine class for retrieval in static methods
	glfwSetWindowUserPointer(gWindow, this);

	glfwSetWindowSizeCallback(gWindow, onWindowResized);
	glfwSetKeyCallback(gWindow, onKeyCallback);
}

void DominusEngine::run()
{
	initWindow();
	initVulkan();
	gameLoop();
	//cleanUp();
}

void DominusEngine::gameLoop()
{
	std::cout << "Entering game loop" << std::endl;

	while (!glfwWindowShouldClose(gWindow)) {
		glfwPollEvents();
		updateUniformBuffer();
		drawFrame();
	}

	vkDeviceWaitIdle(gDevice);
}

void DominusEngine::cleanUp()
{
	std::cout << "Cleaning up" << std::endl;

	cleanupSwapChain();
	vkDestroySampler(gDevice, textureSampler, nullptr);
	vkDestroyImageView(gDevice, textureImageView, nullptr);
	vkDestroyImage(gDevice, textureImage, nullptr);
	vkFreeMemory(gDevice, textureImageMemory, nullptr);
	vkDestroyDescriptorPool(gDevice, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(gDevice, descriptionSetLayout, nullptr);
	uniformBuffer.destroy();
	indexBuffer.destroy();
	vertexBuffer.destroy();
	vkDestroySemaphore(gDevice, renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(gDevice, imageAvailableSemaphore, nullptr);
	vkDestroyCommandPool(gDevice, gDevice.graphicsCommandPool, nullptr);
	vkDestroyDevice(gDevice, nullptr);
	destroyDebugReportCallbackEXT(gInstance, gCallback, nullptr);
	vkDestroySurfaceKHR(gInstance, gSurface, nullptr);
	vkDestroyInstance(gInstance, nullptr);
	glfwDestroyWindow(gWindow);
	glfwTerminate();
}

void DominusEngine::createVKInstance()
{
	std::cout << "Creating Vulkan instance" << std::endl;

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

void DominusEngine::setupDebugCallback()
{
	if (!enableValidationLayers)
		return;

	std::cout << "Setting up Vulkan debug callback" << std::endl;

	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;

	if (createDebugReportCallbackEXT(gInstance, &createInfo, nullptr, &gCallback) != VK_SUCCESS)
		throw std::runtime_error("Failed to set up debug callback!");

}

void DominusEngine::pickPyshicalDevice()
{
	uint32_t deviceCount = 0;

	vkEnumeratePhysicalDevices(gInstance, &deviceCount, nullptr);

	if (deviceCount == 0)
		throw std::runtime_error("Failed to find GPU's with Vulkan support");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(gInstance, &deviceCount, devices.data());

	std::cout << "Available devices (" << deviceCount << ") :" << std::endl;

	for (const auto& device : devices)
	{
		DominusDevice tmp(device);

		if (isDeviceSuitable(device))
		{
			gDevice = tmp;
			break;
		}
	}

	if (gDevice.physicalDevice == VK_NULL_HANDLE)
		throw std::runtime_error("Failed to find a suitable GPU");
}

bool DominusEngine::isDeviceSuitable(DominusDevice aDevice)
{
	VkBool32 presentSupport = false;
	int32_t graphicsQIndex;
	int32_t presentQIndex;

	std::cout << "Checking if device is suitable" << std::endl;

	graphicsQIndex = (int32_t)aDevice.getQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
	presentQIndex = (int32_t)aDevice.getQueueFamilyIndexPresentation(gSurface);

	if (graphicsQIndex == -1 || presentQIndex == -1)
		return false;

	aDevice.queueFamilyIndices.graphics = graphicsQIndex;
	aDevice.queueFamilyIndices.present = presentQIndex;

	for (auto ext : requiredExtensions)
	{
		if (!aDevice.isExtensionSupported(ext))
		{
			std::cout << "Extension not supported " << ext << std::endl;
			return false;
		}
	}

	aDevice.querySwapChainSupport(gSurface);

	if (aDevice.surfaceFormats.empty() || aDevice.surfacePresentModes.empty())
	{
		std::cout << "No formats or present modes available" << std::endl;
		return false;
	}

	if (!aDevice.features.samplerAnisotropy)
	{
		std::cout << "Anistropic filtering not supported" << std::endl;
		return false;
	}

	if (!aDevice.features.sampleRateShading)
	{
		std::cout << "Multisampling not supported" << std::endl;
		return false;
	}

	if (!aDevice.features.fillModeNonSolid)
	{
		std::cout << "Fill mode non solid not supported" << std::endl;
		return false;
	}

	std::cout << "Device is suitable" << std::endl;

	return true;
}

void DominusEngine::createLogicalDevice()
{
	std::cout << "Creating logical device" << std::endl;

	gDevice.querySwapChainSupport(gSurface);
	std::cout << gDevice;

	VkPhysicalDeviceFeatures enableFeatures = {};
	enableFeatures.samplerAnisotropy = VK_TRUE;
	enableFeatures.fillModeNonSolid = VK_TRUE;
	enableFeatures.sampleRateShading = VK_TRUE;

	if (gDevice.createLogicalDevice(enableFeatures, requiredExtensions, VK_QUEUE_GRAPHICS_BIT) != VK_SUCCESS)
		std::runtime_error("Failed to create logical device");

	std::cout << "Getting queue handles" << std::endl;

	vkGetDeviceQueue(gDevice, gDevice.queueFamilyIndices.graphics, 0, &gGraphicsQueue);
	vkGetDeviceQueue(gDevice, gDevice.queueFamilyIndices.present, 0, &gPresentQueue);
}

void DominusEngine::createSwapChain()
{
	std::cout << "Creating swapchain" << std::endl;

	gDevice.querySwapChainSupport(gSurface);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(gDevice.surfaceFormats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(gDevice.surfacePresentModes);
	VkExtent2D extent = chooseSwapExtent(gDevice.capabilities);

	uint32_t imageCount = gDevice.capabilities.minImageCount + 1;

	std::cout << "Swapchain image count: " << imageCount << std::endl;

	if (gDevice.capabilities.maxImageCount > 0 && imageCount > gDevice.capabilities.maxImageCount)
	{
		std::cout << "Setting swapchain image limit to max: " << gDevice.capabilities.maxImageCount << std::endl;
		imageCount = gDevice.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = gSurface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = { static_cast<uint32_t>(gDevice.queueFamilyIndices.graphics),  static_cast<uint32_t>(gDevice.queueFamilyIndices.present) };


	if (gDevice.queueFamilyIndices.graphics != gDevice.queueFamilyIndices.present)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;		// Optional
		createInfo.pQueueFamilyIndices = nullptr;	// Optional
	}

	createInfo.preTransform = gDevice.capabilities.currentTransform;
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

void DominusEngine::createImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());

	std::cout << "Creating image views (" << swapChainImageViews.size() << ")" << std::endl;

	for (uint32_t i = 0; i < swapChainImages.size(); i++)
	{
		swapChainImageViews[i] = createImageView(swapChainImages[i], gSwapChainFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

void DominusEngine::createTextureSampler()
{
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(gDevice, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
		throw std::runtime_error("Failed to create texture sampler!");
}

void DominusEngine::createGraphicsPipeline()
{
	VkShaderModule vertShaderModule = DominusTools::loadShader(gDevice, "shaders/vert.spv");
	VkShaderModule fragShaderModule = DominusTools::loadShader(gDevice, "shaders/frag.spv");

	std::cout << "Creating graphics pipeline" << std::endl;

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)gSwapChainExtent.width;
	viewport.height = (float)gSwapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = gSwapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;	// Optional
	rasterizer.depthBiasClamp = 0.0f;			// Optional
	rasterizer.depthBiasSlopeFactor = 0.0f;		// Optional

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_TRUE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;				// Optional
	multisampling.pSampleMask = nullptr;				// Optional
	multisampling.alphaToCoverageEnable = VK_FALSE;		// Optional
	multisampling.alphaToOneEnable = VK_FALSE;			// Optional

	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f;		// Optional
	depthStencil.maxDepthBounds = 1.0f;		// Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {};				// Optional
	depthStencil.back = {};					// Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;		// Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;	// Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;				// Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;		// Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;	// Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;				// Optional

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;				// Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;					// Optional
	colorBlending.blendConstants[1] = 0.0f;					// Optional
	colorBlending.blendConstants[2] = 0.0f;					// Optional
	colorBlending.blendConstants[3] = 0.0f;					// Optional

	VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptionSetLayout;

	if (vkCreatePipelineLayout(gDevice, &pipelineLayoutInfo, nullptr, &gPipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("Failed to create pipeline layout!");

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = VK_NULL_HANDLE;			// Optional
	pipelineInfo.layout = gPipelineLayout;
	pipelineInfo.renderPass = gRenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;	// Optional
	pipelineInfo.basePipelineIndex = -1;				// Optional
	pipelineInfo.pDepthStencilState = &depthStencil;	// Optional

	// Pipeline cache for constructing other pipelines
	VkPipelineCache pipelineCache;
	VkPipelineCacheCreateInfo pipelineCacheInfo = {};
	pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	pipelineCacheInfo.pNext = nullptr;
	pipelineCacheInfo.flags = 0;
	pipelineCacheInfo.initialDataSize = 0;
	pipelineCacheInfo.pInitialData = nullptr;

	if (vkCreatePipelineCache(gDevice, &pipelineCacheInfo, nullptr, &pipelineCache) != VK_SUCCESS)
		throw std::runtime_error("Failed to create pipeline cache");

	std::cout << "\tCraeted pipeline cache" << std::endl;

	if (vkCreateGraphicsPipelines(gDevice, pipelineCache, 1, &pipelineInfo, nullptr, &pipelines[pipelineModes::SOLID]) != VK_SUCCESS)
		throw std::runtime_error("Failed to create graphics pipeline!");

	std::cout << "\tCraeted solid pipeline" << std::endl;

	rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
	pipelineInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
	pipelineInfo.basePipelineHandle = pipelines[pipelineModes::SOLID];
	pipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(gDevice, pipelineCache, 1, &pipelineInfo, nullptr, &pipelines[pipelineModes::LINE]) != VK_SUCCESS)
		throw std::runtime_error("Failed to create graphics pipeline!");

	std::cout << "\tCraeted line pipeline" << std::endl;

	rasterizer.polygonMode = VK_POLYGON_MODE_POINT;

	if (vkCreateGraphicsPipelines(gDevice, pipelineCache, 1, &pipelineInfo, nullptr, &pipelines[pipelineModes::POINT]) != VK_SUCCESS)
		throw std::runtime_error("Failed to create graphics pipeline!");

	std::cout << "\tCraeted point pipeline" << std::endl;

	vkDestroyPipelineCache(gDevice, pipelineCache, nullptr);
	vkDestroyShaderModule(gDevice, fragShaderModule, nullptr);
	vkDestroyShaderModule(gDevice, vertShaderModule, nullptr);
}

void DominusEngine::createRenderPass()
{
	std::cout << "Creating render pass" << std::endl;

	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = gSwapChainFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(gDevice, &renderPassInfo, nullptr, &gRenderPass) != VK_SUCCESS)
		throw std::runtime_error("Failed to create render pass!");
}

void DominusEngine::createFramebuffers()
{
	gSwapChainFramebuffers.resize(swapChainImageViews.size());

	std::cout << "Creating frame buffers (" << gSwapChainFramebuffers.size() << ")" << std::endl;

	for (size_t i = 0; i < swapChainImageViews.size(); i++)
	{
		std::array<VkImageView, 2> attachments = { swapChainImageViews[i], depthImageView };

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = gRenderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = gSwapChainExtent.width;
		framebufferInfo.height = gSwapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(gDevice, &framebufferInfo, nullptr, &gSwapChainFramebuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create framebuffer!");
	}
}

void DominusEngine::createCommandPool()
{
	std::cout << "Creating command pool" << std::endl;

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = gDevice.queueFamilyIndices.graphics;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;		// Optional

	if (vkCreateCommandPool(gDevice, &poolInfo, nullptr, &gDevice.graphicsCommandPool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create command pool!");
}

void DominusEngine::loadModels()
{
	uint32_t vertexOffset = 0;
	float modelOffset = 0;

	for (auto i = 0; i < 10; i++)
	{
		DominusModel* tmp = new DominusModel(gDevice, vertexBuffer, glm::vec3(modelOffset, 0, 0));
		tmp->vertexOffset = vertexOffset;
		tmp->loadFromFile("meshes/invader.obj");
		modelOffset += 20;
		vertexOffset += 212;

		std::cout << *tmp << std::endl;
		sceneModels.push_back(tmp);
	}
}

void DominusEngine::createVertexBuffer()
{
	// Vertex buffer size needs to be equal to size of all scene component vertex * sizeof(vertex position vector)

	VkDeviceSize bufferSize;
	DominusBuffer stagingBuffer;
	//size_t size;

	std::cout << "Creating vertex buffer" << std::endl;

	for (auto model : sceneModels)
		sceneVertices.insert(sceneVertices.end(), model->vertices.begin(), model->vertices.end());

	bufferSize = sceneVertices.size() * sizeof(Vertex);

	gDevice.createBuffer(stagingBuffer, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	gDevice.createBuffer(vertexBuffer, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	stagingBuffer.transfer(sceneVertices.data());
	gDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize, gGraphicsQueue);
}

void DominusEngine::createIndexBuffer()
{
	// Index buffer size must be equal to indices count * sizeof(single index)

	VkDeviceSize bufferSize;
	DominusBuffer stagingBuffer;

	std::cout << "Creating index buffer" << std::endl;

	for (auto model : sceneModels)
		sceneIndices.insert(sceneIndices.end(), model->indices.begin(), model->indices.end());

	bufferSize = sceneIndices.size() * sizeof(uint32_t);

	gDevice.createBuffer(stagingBuffer, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	gDevice.createBuffer(indexBuffer, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	stagingBuffer.transfer(sceneModels[0]->indices.data());

	gDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize, gGraphicsQueue);
}


void DominusEngine::createCommandBuffers(pipelineModes pipelineMode)
{
	commandBuffers.resize(gSwapChainFramebuffers.size());

	std::cout << "Creating command buffers (" << gSwapChainFramebuffers.size() << ")" << std::endl;

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = gDevice.graphicsCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(gDevice, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate command buffers!");

	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr;		// Optional.

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to begin command buffer");

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = gRenderPass;
		renderPassInfo.framebuffer = gSwapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = gSwapChainExtent;
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[pipelineMode]);

		VkDeviceSize offsets[1] = { 0 };

		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &vertexBuffer.buffer, offsets);
		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

		for (auto model : sceneModels)
		{
			vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, gPipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
			//vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(sceneModels[0]->indices.size()), 1, 0, 0, 0);
			model->draw(&commandBuffers[i]);
		}

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!");
	}
}

void DominusEngine::createUniformBuffer()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	std::cout << "Creating uniform buffer" << std::endl;

	gDevice.createBuffer(uniformBuffer, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	camera.setTranslation(glm::vec3(0.0f, -40.0f, 0.0f));
	camera.setPerspective(90.0f, gSwapChainExtent.width / (float)gSwapChainExtent.height, 0.01f, 100.0f);
	camera.setLookAt(glm::vec3(0.0f, 0.0f, 0.0f));

	ubo.model = glm::mat4(1.0f);
	ubo.view = camera.view;
	ubo.proj = camera.perspective;
}

void DominusEngine::createSempahores()
{
	std::cout << "Creating semaphores" << std::endl;

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(gDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS || vkCreateSemaphore(gDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS)
		throw std::runtime_error("Failed to create semaphores!");
}

void DominusEngine::createDescriptorPool()
{
	std::cout << "Creating descriptor pool" << std::endl;

	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 1;

	if (vkCreateDescriptorPool(gDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create descriptor pool!");
}

void DominusEngine::createDescriptionSetLayout()
{
	std::cout << "Creating descriptor set layout" << std::endl;

	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;	// Optional

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(gDevice, &layoutInfo, nullptr, &descriptionSetLayout) != VK_SUCCESS)
		throw std::runtime_error("Failed to create descriptor set layout!");
}

void DominusEngine::createDescriptorSet()
{
	std::cout << "Creating descriptor set" << std::endl;

	VkDescriptorSetLayout layouts[] = { descriptionSetLayout };
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts;

	if (vkAllocateDescriptorSets(gDevice, &allocInfo, &descriptorSet) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate descriptor set!");

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniformBuffer.buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = textureImageView;
	imageInfo.sampler = textureSampler;

	std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = descriptorSet;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = descriptorSet;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(gDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void DominusEngine::createSurface()
{
	std::cout << "Creating surface" << std::endl;

	if (glfwCreateWindowSurface(gInstance, gWindow, nullptr, &gSurface) != VK_SUCCESS)
		throw std::runtime_error("Failed to create window surface!");
}

void DominusEngine::createTextureImage()
{
	int texWidth, texHeight, texChannels;

	stbi_uc* pixels = stbi_load(TEXTURE.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	std::cout << "Loading texture image: " << TEXTURE << std::endl;
	std::cout << "\tResolution: " << texWidth << "x" << texHeight << std::endl;
	std::cout << "\tSize: " << imageSize << " bytes" << std::endl;

	if (!pixels)
		throw std::runtime_error("Failed to load texture image");

	DominusBuffer stagingBuffer;
	gDevice.createBuffer(stagingBuffer, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer.transfer(pixels);

	stbi_image_free(pixels);

	createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(stagingBuffer.buffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void DominusEngine::createDepthResources()
{
	std::cout << "Creating depth resources" << std::endl;

	VkFormat depthFormat = findDepthFormat();

	createImage(gSwapChainExtent.width, gSwapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
	depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

	transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void DominusEngine::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	std::cout << "Creating image texture" << std::endl;
	std::cout << "\tResolution: " << width << "x" << height << std::endl;
	std::cout << "\tFormat: " << format << std::endl;
	std::cout << "\tUsage: " << usage << std::endl;

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0;	// Optional

	if (vkCreateImage(gDevice, &imageInfo, nullptr, &image) != VK_SUCCESS)
		throw std::runtime_error("Failed to create image!");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(gDevice, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = gDevice.findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(gDevice, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate image memory");

	vkBindImageMemory(gDevice, image, imageMemory, 0);
}

void DominusEngine::drawFrame()
{
	uint32_t imageIndex;
	VkSwapchainKHR swapChains[] = { gSwapChain };
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };

	VkResult result = vkAcquireNextImageKHR(gDevice, gSwapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		std::string error = "Failed to aquire swap chain image!";
		throw std::runtime_error(error);
	}

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if ((result = vkQueueSubmit(gGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE)) != VK_SUCCESS)
	{
		std::string error = "Failed to submit to queue! ";
		error.append(DominusTools::vkResultToString(result));
		throw std::runtime_error(error);
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;		// Optional

	result = vkQueuePresentKHR(gPresentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		recreateSwapChain();
	else if (result != VK_SUCCESS)
		throw std::runtime_error("failed to present swap chain image!");

	vkQueueWaitIdle(gPresentQueue);
}

void DominusEngine::recreateSwapChain()
{
	std::cout << "Recreating swapchain" << std::endl;

	vkDeviceWaitIdle(gDevice);

	cleanupSwapChain();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createDepthResources();
	createFramebuffers();
	createCommandBuffers();
}

void DominusEngine::cleanupSwapChain()
{
	std::cout << "Cleaning up swap chain" << std::endl;

	vkDestroyImageView(gDevice, depthImageView, nullptr);
	vkDestroyImage(gDevice, depthImage, nullptr);
	vkFreeMemory(gDevice, depthImageMemory, nullptr);

	for (auto buffer : gSwapChainFramebuffers)
		vkDestroyFramebuffer(gDevice, buffer, nullptr);


	vkFreeCommandBuffers(gDevice, gDevice.graphicsCommandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

	for (auto pipeline : pipelines)
		vkDestroyPipeline(gDevice, pipeline, nullptr);

	vkDestroyPipelineLayout(gDevice, gPipelineLayout, nullptr);
	vkDestroyRenderPass(gDevice, gRenderPass, nullptr);

	for (VkImageView view : swapChainImageViews) {
		vkDestroyImageView(gDevice, view, nullptr);
	}

	vkDestroySwapchainKHR(gDevice, gSwapChain, nullptr);
}

void DominusEngine::updateUniformBuffer()
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();

	deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	ubo.proj = camera.perspective;
	ubo.view = camera.view;

	uniformBuffer.bufferSize = sizeof(ubo);
	uniformBuffer.transfer(&ubo);
}

bool DominusEngine::checkValidationLayerSupport()
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

void DominusEngine::createTextureImageView()
{
	textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
}

void DominusEngine::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	std::cout << "Transitioning image" << std::endl;

	VkPipelineStageFlags srcStage;
	VkPipelineStageFlags dstStage;
	VkCommandBuffer commandBuffer = gDevice.beginSingleTimeCommands();

	//gDevice.createCommandBuffer(commandBuffer);

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (hasStencilComponent(format))
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	else
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else
		throw std::runtime_error("Unsupported layout transition!");

	vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	//gDevice.submitCommandBuffer(commandBuffer, gGraphicsQueue);

	gDevice.endSingleTimeCommands(commandBuffer, gGraphicsQueue);
}

void DominusEngine::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	std::cout << "Copying buffer to image" << std::endl;

	VkCommandBuffer commandBuffer = gDevice.beginSingleTimeCommands();
	//gDevice.createCommandBuffer(commandBuffer);

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0,0,0 };
	region.imageExtent = { width, height, 1 };

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	gDevice.endSingleTimeCommands(commandBuffer, gGraphicsQueue);
}

VkFormat DominusEngine::findDepthFormat()
{
	std::cout << "Finding depth format" << std::endl;

	std::vector<VkFormat> formats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };

	return findSupportedFormat(formats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool DominusEngine::hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat DominusEngine::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	std::cout << "Finding format" << std::endl;

	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(gDevice.physicalDevice, format, &props);


		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("Failed to find supported format!");
}

VkImageView DominusEngine::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(gDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}

VkSurfaceFormatKHR DominusEngine::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	std::cout << "Picking swapchain format" << std::endl;

	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		std::cout << "No prefered format" << std::endl;
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& format : availableFormats)
	{
		if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			std::cout << "Found specified formats:" << std::endl;
			std::cout << "\t" << VK_FORMAT_B8G8R8A8_UNORM << std::endl;
			std::cout << "\t" << VK_COLOR_SPACE_SRGB_NONLINEAR_KHR << std::endl;
			return format;
		}
	}

	std::cout << "Failed to get specific format returning first format: " << std::endl;
	std::cout << availableFormats[0].format << std::endl;

	return availableFormats[0];
}

VkPresentModeKHR DominusEngine::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availableModes)
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

VkExtent2D DominusEngine::chooseSwapExtent(const VkSurfaceCapabilitiesKHR & capabilities)
{
	std::cout << "Choosing swapchain extent" << std::endl;

	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		std::cout << "Matching window resolution" << std::endl;
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;

		std::cout << "No preferred resolution checking min and max based on capabilities" << std::endl;

		glfwGetWindowSize(gWindow, &width, &height);

		VkExtent2D actualExtent =
		{
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

void DominusEngine::onKeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	DominusEngine* app = reinterpret_cast<DominusEngine*>(glfwGetWindowUserPointer(window));

#ifdef _DEBUG
	std::cout << "Input detected: " << (char)key << " - " << action << std::endl;
#endif

	if (key == GLFW_KEY_ESCAPE && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	else if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		app->camera.translate(glm::vec3(0.0f, app->deltaTime, 0.0f));
	}
	else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		app->camera.translate(glm::vec3(app->deltaTime, 0.0f, 0.0f));
	}
	else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		app->camera.translate(glm::vec3(0.0f, app->deltaTime, 0.0f));
	}
	else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		app->camera.translate(glm::vec3(app->deltaTime, 0.0f, 0.0f));
	}
	else if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		app->camera.translate(glm::vec3(0.0f, 0.0f, app->deltaTime));
	}
	else if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		app->camera.translate(glm::vec3(0.0f, 0.0f, app->deltaTime));
	}
	else if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		app->createCommandBuffers(pipelineModes::SOLID);
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		app->createCommandBuffers(pipelineModes::LINE);
	}
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		app->createCommandBuffers(pipelineModes::POINT);
	}
}

void DominusEngine::onWindowResized(GLFWwindow * window, int width, int height)
{
	std::cout << "Window resize detected" << std::endl;
	std::cout << "\tWidth: " << width << std::endl;
	std::cout << "\tHeight: " << height << std::endl;

	if (width <= 0 || height <= 0)
		return;

	DominusEngine* app = reinterpret_cast<DominusEngine*>(glfwGetWindowUserPointer(window));
	app->recreateSwapChain();
}

std::vector<const char*> DominusEngine::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	std::vector<const char*> extensions;

	std::cout << "Retrieving available GLFW Vulkan extensions" << std::endl;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::cout << "Available GLFW extensions (" << glfwExtensionCount << ") :" << std::endl;

	for (uint32_t i = 0; i < glfwExtensionCount; i++)
	{
		std::cout << "\t" << glfwExtensions[i] << std::endl;
		extensions.push_back(glfwExtensions[i]);
	}

	if (enableValidationLayers)
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	return extensions;
}

void DominusEngine::glfwErrorCallback(int error, const char * description)
{
	puts(description);
}

void DominusEngine::destroyDebugReportCallbackEXT(VkInstance aInstance, VkDebugReportCallbackEXT aCallback, const VkAllocationCallbacks * aAllocator)
{
	std::cout << "destroying vulkan debug callback function" << std::endl;

	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(aInstance, "vkDestroyDebugReportCallbackEXT");

	if (func != nullptr)
		func(aInstance, aCallback, aAllocator);
}

VkResult DominusEngine::createDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
{
	std::cout << "Creating vulkan debug callback function" << std::endl;

	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");

	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pCallback);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

VKAPI_ATTR VkBool32 VKAPI_CALL DominusEngine::debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char * layerPrefix, const char * msg, void * userData)
{
	std::cerr << "Error : " << code << " at " << location << " - validation layer: " << msg << std::endl;

	return VK_FALSE;
}