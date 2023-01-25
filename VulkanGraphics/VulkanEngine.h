#pragma once
#include <SDL.h>

#include <iostream>
#include "SDL_vulkan.h"
#include <vulkan/vulkan.h>
#include <vector>
#include "vkbootstrap/VkBootstrap.h"

import Helper;

class VulkanEngine
{
private:
	struct SDL_Window* m_window{ nullptr };

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
	void Draw();
#pragma endregion

public:
	VulkanEngine();
	VulkanEngine(const VulkanEngine&) = delete;
	VulkanEngine(const VulkanEngine&&) = delete;
	void Init();
	void Run();
	void CleanUp();
};