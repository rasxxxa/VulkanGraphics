#include <fstream>
#include <vulkan/vulkan.h>
#include "VulkanEngine.h"
#include "VKInit.h"
#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"


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

	m_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	m_graphicsQueueFamilyIndex = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = m_physicalDevice;
	allocatorInfo.device = m_logicalDevice;
	allocatorInfo.instance = m_instance;
	VK_CHECK(vmaCreateAllocator(&allocatorInfo, &m_allocator));
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
	m_swapchainImageFormat = vkbSwapchain.image_format;
	m_swapChainImages.images = vkbSwapchain.get_images().value();
	m_swapChainImages.imageViews = vkbSwapchain.get_image_views().value();

	for (int i = 0; i < m_swapChainImages.imageViews.size(); i++)
	{
		m_deleter.Push([this, frame = i]() {vkDestroyImageView(m_logicalDevice, m_swapChainImages.imageViews[frame], nullptr); });
	}
	m_deleter.Push([this]() {vkDestroySwapchainKHR(m_logicalDevice, m_swapchain, nullptr); });

}

void VulkanEngine::InitCommands()
{
	VkCommandPoolCreateInfo commandPoolCreateInfo = VkInit::CommandPoolCreateInfo(m_graphicsQueueFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	VK_CHECK(vkCreateCommandPool(m_logicalDevice, &commandPoolCreateInfo, nullptr, &m_commandPool));
	m_deleter.Push([this]() {vkDestroyCommandPool(m_logicalDevice, m_commandPool, nullptr); });

	VkCommandBufferAllocateInfo commandBufferAllocateInfo = VkInit::CommandBufferAllocateInfo(m_commandPool, 1);
	
	VK_CHECK(vkAllocateCommandBuffers(m_logicalDevice, &commandBufferAllocateInfo, &m_commandBuffer));
}

void VulkanEngine::InitDefaultRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = m_swapchainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;   // We clear when attacment is loaded
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // We keep the attachment stored when the renderpass finished
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // we don't care about stencil
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	// we don't care about starting layout of attachment
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	// final layout has to be in layout ready for display
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	VkAttachmentReference color_attachment_ref = {};
	//attachment number will index into the pAttachments array in the parent renderpass itself
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//we are going to create 1 subpass, which is the minimum you can do
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;

	//connect the color attachment to the info
	render_pass_info.attachmentCount = 1;
	render_pass_info.pAttachments = &colorAttachment;
	//connect the subpass to the info
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;


	VK_CHECK(vkCreateRenderPass(m_logicalDevice, &render_pass_info, nullptr, &m_renderPass));
	m_deleter.Push([this]() {vkDestroyRenderPass(m_logicalDevice, m_renderPass, nullptr); });
}

void VulkanEngine::InitFrameBuffers()
{
	VkFramebufferCreateInfo fb_info = {};
	fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fb_info.pNext = nullptr;

	fb_info.renderPass = m_renderPass;
	fb_info.attachmentCount = 1;
	fb_info.width = WindowSize.width;
	fb_info.height = WindowSize.height;
	fb_info.layers = 1;

	//grab how many images we have in the swapchain
	const uint32_t swapchain_imagecount = m_swapChainImages.images.size();
	m_frameBuffers = std::vector<VkFramebuffer>(swapchain_imagecount);

	//create framebuffers for each of the swapchain image views
	for (int i = 0; i < swapchain_imagecount; i++) {

		fb_info.pAttachments = &m_swapChainImages.imageViews[i];
		VK_CHECK(vkCreateFramebuffer(m_logicalDevice, &fb_info, nullptr, &m_frameBuffers[i]));
		m_deleter.Push([this, buffer = i]() {vkDestroyFramebuffer(m_logicalDevice, m_frameBuffers[buffer], nullptr); });
	}
}

void VulkanEngine::InitSyncStructures()
{
	//create synchronization structures

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;

	//we want to create the fence with the Create Signaled flag, so we can wait on it before using it on a GPU command (for the first frame)
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VK_CHECK(vkCreateFence(m_logicalDevice, &fenceCreateInfo, nullptr, &m_renderFence));
	m_deleter.Push([this]() {vkDestroyFence(m_logicalDevice, m_renderFence, nullptr); });

	//for the semaphores we don't need any flags
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;

	VK_CHECK(vkCreateSemaphore(m_logicalDevice, &semaphoreCreateInfo, nullptr, &m_presentSemaphore));
	m_deleter.Push([this]() {vkDestroySemaphore(m_logicalDevice, m_presentSemaphore, nullptr); });
	VK_CHECK(vkCreateSemaphore(m_logicalDevice, &semaphoreCreateInfo, nullptr, &m_renderSemaphore));
	m_deleter.Push([this]() {vkDestroySemaphore(m_logicalDevice, m_renderSemaphore, nullptr); });
}

#include <filesystem>

bool VulkanEngine::LoadShaderModule(const std::string& path, VkShaderModule* createdShaderModule)
{
	//open the file. With cursor at the end
	std::ifstream file(path.c_str(), std::ios::ate | std::ios::binary);
	if (!file.is_open()) {
		return false;
	}

	size_t fileSize = (size_t)file.tellg();

	//spirv expects the buffer to be on uint32, so make sure to reserve an int vector big enough for the entire file
	std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

	//put file cursor at beginning
	file.seekg(0);

	//load the entire file into the buffer
	file.read((char*)buffer.data(), fileSize);

	//now that the file is loaded into the buffer, we can close it
	file.close();

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;

	//codeSize has to be in bytes, so multiply the ints in the buffer by size of int to know the real size of the buffer
	createInfo.codeSize = buffer.size() * sizeof(uint32_t);
	createInfo.pCode = buffer.data();

	//check that the creation goes well.
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		return false;
	}
	*createdShaderModule = shaderModule;
	return true;
}

void VulkanEngine::InitPipelines()
{
	VkShaderModule fragShader, vertShader;
	if (!LoadShaderModule("../../../../VulkanGraphics/Shaders/frag.spv", &fragShader))
	{
		std::cout << "Error loading frag shader" << std::endl;
	}
	else
	{
		std::cout << "Frag shader loaded" << std::endl;
	}
	
	if (!LoadShaderModule("../../../../VulkanGraphics/Shaders/vert.spv", &vertShader))
	{
		std::cout << "Error loading vert shader" << std::endl;
	}
	else
	{
		std::cout << "Vert shader loaded" << std::endl;
	}

	//build the pipeline layout that controls the inputs/outputs of the shader
    //we are not using descriptor sets or other systems yet, so no need to use anything other than empty default
	VkPipelineLayoutCreateInfo pipeline_layout_info = VkInit::PipelineLayoutCreateInfo();

	VK_CHECK(vkCreatePipelineLayout(m_logicalDevice, &pipeline_layout_info, nullptr, &m_pipelineLayout));
	m_deleter.Push([this]() { vkDestroyPipelineLayout(m_logicalDevice, m_pipelineLayout, nullptr); });

	// layout and shader modules creation

	//build the stage-create-info for both vertex and fragment stages. This lets the pipeline know the shader modules per stage

	m_shaderStages.push_back(
		VkInit::PipeLineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertShader));

	m_shaderStages.push_back(
		VkInit::PipeLineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragShader));


	//vertex input controls how to read vertices from vertex buffers. We aren't using it yet
	m_vertexInputInfo = VkInit::VertexInputStateCreateInfo();

	//input assembly is the configuration for drawing triangle lists, strips, or individual points.
	//we are just going to draw triangle list
	m_inputAssembly = VkInit::InputAssemblyCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

	//build viewport and scissor from the swapchain extents
	m_viewport.x = 0.0f;
	m_viewport.y = 0.0f;
	VkExtent2D _windowExtent;
	_windowExtent.height = WindowSize.height;
	_windowExtent.width = WindowSize.width;
	m_viewport.width = (float)_windowExtent.width;
	m_viewport.height = (float)_windowExtent.height;
	m_viewport.minDepth = 0.0f;
	m_viewport.maxDepth = 1.0f;
	m_scissor.offset = { 0, 0 };
	m_scissor.extent = _windowExtent;

	//configure the rasterizer to draw filled triangles
	m_rasterizer = VkInit::RasterisationStateCreateInfo(VK_POLYGON_MODE_FILL);

	//we don't use multisampling, so just run the default one
	m_multisampling = VkInit::MultiSamplingCreateInfo();

	//a single blend attachment with no blending and writing to RGBA
	m_colorBlendAttachment = VkInit::ColorBlendAttachmentState();

	//finally build the pipeline
	m_pipeline = BuildPipeline(m_logicalDevice, m_renderPass);
	m_deleter.Push([this]() { vkDestroyPipeline(m_logicalDevice, m_pipeline, nullptr); });


	vkDestroyShaderModule(m_logicalDevice, fragShader, nullptr);
	vkDestroyShaderModule(m_logicalDevice, vertShader, nullptr);
}

void VulkanEngine::Draw()
{
	//wait until the GPU has finished rendering the last frame. Timeout of 1 second
	VK_CHECK(vkWaitForFences(m_logicalDevice, 1, &m_renderFence, true, 1000000000));
	VK_CHECK(vkResetFences(m_logicalDevice, 1, &m_renderFence));

	uint32_t swapchainImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(m_logicalDevice, m_swapchain, 1000000000, m_presentSemaphore, nullptr, &swapchainImageIndex));

	//now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
	VK_CHECK(vkResetCommandBuffer(m_commandBuffer, 0));

	//naming it cmd for shorter writing
	VkCommandBuffer cmd = m_commandBuffer;

	//begin the command buffer recording. We will use this command buffer exactly once, so we want to let Vulkan know that
	VkCommandBufferBeginInfo cmdBeginInfo = {};
	cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmdBeginInfo.pNext = nullptr;

	cmdBeginInfo.pInheritanceInfo = nullptr;
	cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	//make a clear-color from frame number. This will flash with a 120*pi frame period.
	VkClearValue clearValue;
	float flash = abs(sin(m_frameNumber / 120.f));
	clearValue.color = { { 0.0f, 0.0f, flash, 1.0f } };

	//start the main renderpass.
	//We will use the clear color from above, and the framebuffer of the index the swapchain gave us
	VkRenderPassBeginInfo rpInfo = {};
	rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rpInfo.pNext = nullptr;

	rpInfo.renderPass = m_renderPass;
	rpInfo.renderArea.offset.x = 0;
	rpInfo.renderArea.offset.y = 0;
	VkExtent2D _windowExtent;
	_windowExtent.width = WindowSize.width;
	_windowExtent.height = WindowSize.height;
	rpInfo.renderArea.extent = _windowExtent;
	rpInfo.framebuffer = m_frameBuffers[swapchainImageIndex];

	//connect clear values
	rpInfo.clearValueCount = 1;
	rpInfo.pClearValues = &clearValue;

	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
	vkCmdDraw(cmd, 3, 1, 0, 0);

	vkCmdEndRenderPass(cmd);

	VK_CHECK(vkEndCommandBuffer(m_commandBuffer));

	VkSubmitInfo submit = {};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.pNext = nullptr;

	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	submit.pWaitDstStageMask = &waitStage;

	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &m_presentSemaphore;

	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &m_renderSemaphore;

	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &cmd;

	//submit command buffer to the queue and execute it.
	// _renderFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit(m_graphicsQueue, 1, &submit, m_renderFence));

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;

	presentInfo.pSwapchains = &m_swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &m_renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	VK_CHECK(vkQueuePresentKHR(m_graphicsQueue, &presentInfo));

	//increase the number of frames drawn
	m_frameNumber++;
}



VkPipeline VulkanEngine::BuildPipeline(VkDevice device, VkRenderPass pass)
{
	//make viewport state from our stored viewport and scissor.
    //at the moment we won't support multiple viewports or scissors
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.pNext = nullptr;

	viewportState.viewportCount = 1;
	viewportState.pViewports = &m_viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &m_scissor;

	//setup dummy color blending. We aren't using transparent objects yet
	//the blending is just "no blend", but we do write to the color attachment
	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.pNext = nullptr;

	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &m_colorBlendAttachment;

	//build the actual pipeline
	//we now use all of the info structs we have been writing into into this one to create the pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = nullptr;

	pipelineInfo.stageCount = m_shaderStages.size();
	pipelineInfo.pStages = m_shaderStages.data();
	pipelineInfo.pVertexInputState = &m_vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &m_inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &m_rasterizer;
	pipelineInfo.pMultisampleState = &m_multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = m_pipelineLayout;
	pipelineInfo.renderPass = pass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	//it's easy to error out on create graphics pipeline, so we handle it a bit better than the common VK_CHECK case
	VkPipeline newPipeline;
	if (vkCreateGraphicsPipelines(
		device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS) {
		std::cout << "failed to create pipeline\n";
		return VK_NULL_HANDLE; // failed to create graphics pipeline
	}
	else
	{
		return newPipeline;
	}
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

	InitCommands();

	InitDefaultRenderPass();

	InitFrameBuffers();

	InitSyncStructures();

	InitPipelines();
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
			if (e.type == SDL_QUIT) bQuit = true;
		}
		Draw();
	}
}

void VulkanEngine::CleanUp()
{
	vkWaitForFences(m_logicalDevice, 1, &m_renderFence, true, 1000000000);

	m_deleter.Flush();

	vkDestroyDevice(m_logicalDevice, nullptr);
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkb::destroy_debug_utils_messenger(m_instance, m_debugMessenger);
	vkDestroyInstance(m_instance, nullptr);
	SDL_DestroyWindow(m_window);
}

#pragma region Notes

//Transfer - only queue families in Vulkan are very interesting, as they are used to perform background asset uploadand
//they often can run completely asynchronously from rendering.
//So if you want to upload assets to the GPU from a background thread, 
//using a Transfer - only queue will be a great choice.

#pragma endregion

