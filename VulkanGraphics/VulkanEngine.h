#pragma once
#include <SDL.h>
#include <vulkan/vulkan.h>
#include "vkbootstrap/VkBootstrap.h"

import Helper;

class VulkanEngine
{
private:
	struct SDL_Window* _window{ nullptr };

#pragma region VulkanCoreStruct
	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkPhysicalDevice m_physicalDevice;
	VkDevice m_logicalDevice;
	VkSurfaceKHR m_surface;
#pragma endregion

#pragma region VulkanCoreFunction

	void InitVulkan();

#pragma endregion

public:
	VulkanEngine();
	VulkanEngine(const VulkanEngine&) = delete;
	VulkanEngine(const VulkanEngine&&) = delete;
	void Init();
	void Run();
	void CleanUp();
};