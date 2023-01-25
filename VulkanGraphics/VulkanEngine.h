#pragma once
#include <SDL.h>
#include <vulkan/vulkan.h>
import Helper;

class VulkanEngine
{
private:
	struct SDL_Window* _window{ nullptr };
public:
	VulkanEngine();
	VulkanEngine(const VulkanEngine&) = delete;
	VulkanEngine(const VulkanEngine&&) = delete;
	void Init();
	void Run();
	void CleanUp();
};