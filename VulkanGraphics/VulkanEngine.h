#pragma once
#include <SDL.h>
#include <vulkan/vulkan.h>
#include "vkbootstrap/VkBootstrap.h"
#include "SDL_vulkan.h"
#include <vector>


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
#pragma endregion

#pragma region VulkanCoreFunction

	void InitVulkan();
	void InitSwapchain();
#pragma endregion

public:
	VulkanEngine();
	VulkanEngine(const VulkanEngine&) = delete;
	VulkanEngine(const VulkanEngine&&) = delete;
	void Init();
	void Run();
	void CleanUp();
};