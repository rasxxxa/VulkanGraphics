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
		.require_api_version(VkVersion.major, VkVersion.minor, 0);

	if (RequestValidation)
	{
		builder.use_default_debug_messenger();
	}

	auto instance = builder.build();
	auto vkb_instance = instance.value();
	m_instance = vkb_instance.instance;
	m_debugMessenger = vkb_instance.debug_messenger;


	SDL_Vulkan_CreateSurface(m_window, m_instance, &m_surface);

	vkb::PhysicalDeviceSelector selector{ vkb_instance };
	vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version(VkVersion.major, VkVersion.minor)
		.set_surface(m_surface)
		.select()
		.value();

	vkb::DeviceBuilder deviceBuilder(physicalDevice);
	vkb::Device vkbDevice = deviceBuilder.build().value();

	m_logicalDevice = vkbDevice.device;
	m_physicalDevice = physicalDevice.physical_device;
}



void VulkanEngine::InitSwapchain()
{
	// If we need window resize, rebuild swapchain TODO
	vkb::SwapchainBuilder swapchainBuilder{ m_physicalDevice, m_logicalDevice, m_surface };
	vkb::Swapchain vkbSwapchain = swapchainBuilder
		.use_default_format_selection()
		.set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
		.set_desired_extent(WindowSize.width, WindowSize.height)
		.build()
		.value();

	m_swapchain = vkbSwapchain.swapchain;
	m_swapChainImages.images = vkbSwapchain.get_images().value();
	m_swapChainImages.imageViews = vkbSwapchain.get_image_views().value();
}



VulkanEngine::VulkanEngine()
{
}

void VulkanEngine::Init()
{
	// We initialize SDL and create a window with it. 
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

	m_window = SDL_CreateWindow(
		"Vulkan Engine",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WindowSize.width,
		WindowSize.height,
		window_flags
	);

	InitVulkan();

	InitSwapchain();
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
	vkDestroySwapchainKHR(m_logicalDevice, m_swapchain, nullptr);

	for (auto& val : m_swapChainImages.imageViews)
	{
		vkDestroyImageView(m_logicalDevice, val, nullptr);
	}

	vkDestroyDevice(m_logicalDevice, nullptr);
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkb::destroy_debug_utils_messenger(m_instance, m_debugMessenger);
	vkDestroyInstance(m_instance, nullptr);
	SDL_DestroyWindow(m_window);
}
