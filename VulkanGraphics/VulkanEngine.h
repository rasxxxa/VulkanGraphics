#pragma once
#include <SDL.h>

#include <iostream>

#include "SDL_vulkan.h"
#include <vulkan/vulkan.h>
#include <vector>
#include "vma/vk_mem_alloc.h"
#include "vkbootstrap/VkBootstrap.h"
#include "DeletionQueue.h"
#include "Mesh.h"

import Helper;

class VulkanEngine
{
private:
	struct SDL_Window* m_window{ nullptr };
#pragma region DepthImage
	VkImageView m_depthImageView;
	AllocatedImage m_depthImage;
	VkFormat m_depthFormat;
#pragma endregion
#pragma region MESHES
	Mesh mesh;
#pragma endregion

#pragma region Deletor
	DeletionQueue m_deleter;
#pragma endregion

#pragma region Allocator
	VmaAllocator m_allocator;
#pragma endregion

#pragma region VulkanCoreStruct
	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkPhysicalDevice m_physicalDevice;
	VkDevice m_logicalDevice;
	VkSurfaceKHR m_surface;

	VkSwapchainKHR m_swapchain;
	VkFormat m_swapchainImageFormat;

	struct SwapChainImagesProperties
	{
		std::vector<VkImage> images;
		std::vector<VkImageView> imageViews;
	};

	SwapChainImagesProperties m_swapChainImages;

	VkQueue m_graphicsQueue;
	uint32_t m_graphicsQueueFamilyIndex;

	VkCommandPool m_commandPool;
	VkCommandBuffer m_commandBuffer;

	VkRenderPass m_renderPass;
	std::vector<VkFramebuffer> m_frameBuffers;

	VkSemaphore m_presentSemaphore, m_renderSemaphore;
	VkFence m_renderFence;

#pragma endregion

#pragma region PrimitiveTypes
	int m_frameNumber = 0;
#pragma endregion

#pragma region VulkanCoreFunction

	void InitVulkan();
	void InitSwapchain();
	void InitCommands();
	void InitDefaultRenderPass();
	void InitFrameBuffers();
	void InitSyncStructures();
	bool LoadShaderModule(const std::string& path, VkShaderModule* createdShaderModule);
	void InitPipelines();
	void Draw();
	void LoadMesh();

	void UploadMesh(Mesh& mesh);
#pragma endregion

#pragma region Pipeline
	std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;
	VkPipelineVertexInputStateCreateInfo m_vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo m_inputAssembly;
	VkViewport m_viewport;
	VkRect2D m_scissor;
	VkPipelineRasterizationStateCreateInfo m_rasterizer;
	VkPipelineColorBlendAttachmentState m_colorBlendAttachment;
	VkPipelineMultisampleStateCreateInfo m_multisampling;
	VkPipeline BuildPipeline(VkDevice device, VkRenderPass pass);
	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_pipeline;
	VkPipelineDepthStencilStateCreateInfo m_depthStencil;
#pragma endregion

public:
	VulkanEngine();
	VulkanEngine(const VulkanEngine&) = delete;
	VulkanEngine(const VulkanEngine&&) = delete;
	void Init();
	void Run();
	void CleanUp();
};