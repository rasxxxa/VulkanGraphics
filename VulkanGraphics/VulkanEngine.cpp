#include "VulkanEngine.h"

#define VK_CHECK(x)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			std::cout <<"Detected Vulkan error: " << err << std::endl; \
			abort();                                                \
		}                                                           \
	} while (0)

void VulkanEngine::InitVulkan()
{
	// using vkBoostrap to create first instance
	vkb::InstanceBuilder builder;
	builder
		.set_app_name("Engine")
		.request_validation_layers(RequestValidation)
		.require_api_version(1, 1, 0);

	if (RequestValidation)
	{
		builder.use_default_debug_messenger();
	}

	auto instance = builder.build();
	auto vkb_instance = instance.value();
	m_instance = vkb_instance.instance;
	m_debugMessenger = vkb_instance.debug_messenger;
}

VulkanEngine::VulkanEngine()
{
}

void VulkanEngine::Init()
{
	// We initialize SDL and create a window with it. 
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

	_window = SDL_CreateWindow(
		"Vulkan Engine",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WindowSize.width,
		WindowSize.height,
		window_flags
	);

	InitVulkan();

}

void VulkanEngine::Run()
{
	SDL_Event e;
	bool bQuit = false;

	//main loop
	while (!bQuit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{

		}
	}
}

void VulkanEngine::CleanUp()
{
}
