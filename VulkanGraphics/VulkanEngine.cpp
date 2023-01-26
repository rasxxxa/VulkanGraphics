#include <fstream>
#include <vulkan/vulkan.h>
#include "VulkanEngine.h"
#include "VKInit.h"
#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>

#include <SDL_vulkan.h>
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

VulkanEngine::Frame& VulkanEngine::GetCurrentFrame()
{
	return m_frames[m_frameNumber % FRAMES];
}

AllocatedBuffer VulkanEngine::CreateBuffer(size_t allocSize, VkBufferUsageFlagBits usage, VmaMemoryUsage memoryUsage)
{
	//allocate vertex buffer
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = nullptr;

	bufferInfo.size = allocSize;
	bufferInfo.usage = usage;


	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = memoryUsage;

	AllocatedBuffer newBuffer;

	//allocate the buffer
	VK_CHECK(vmaCreateBuffer(m_allocator, &bufferInfo, &vmaallocInfo,
		&newBuffer.buffer,
		&newBuffer.allocation,
		nullptr));

	return newBuffer;
}

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

	m_frames.resize(FRAMES);
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

	//depth image size will match the window
	VkExtent3D depthImageExtent = {
		WindowSize.width,
		WindowSize.height,
		1
	};

	//hardcoding the depth format to 32 bit float
	m_depthFormat = VK_FORMAT_D32_SFLOAT;

	//the depth image will be an image with the format we selected and Depth Attachment usage flag
	VkImageCreateInfo dimg_info = VkInit::ImageCreateInfo(m_depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImageExtent);

	//for the depth image, we want to allocate it from GPU local memory
	VmaAllocationCreateInfo dimg_allocinfo = {};
	dimg_allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	dimg_allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	//allocate and create the image
	vmaCreateImage(m_allocator, &dimg_info, &dimg_allocinfo, &m_depthImage.image, &m_depthImage.allocation, nullptr);

	//build an image-view for the depth image to use for rendering
	VkImageViewCreateInfo dview_info = VkInit::ImageViewCreateInfo(m_depthFormat, m_depthImage.image, VK_IMAGE_ASPECT_DEPTH_BIT);

	VK_CHECK(vkCreateImageView(m_logicalDevice, &dview_info, nullptr, &m_depthImageView));

	//add to deletion queues
	m_deleter.Push([this]() {
		vkDestroyImageView(m_logicalDevice, m_depthImageView, nullptr);
		vmaDestroyImage(m_allocator, m_depthImage.image, m_depthImage.allocation);
		});

}

void VulkanEngine::InitCommands()
{
	VkCommandPoolCreateInfo commandPoolCreateInfo = VkInit::CommandPoolCreateInfo(m_graphicsQueueFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	for (int frame = 0; frame < FRAMES; frame++)
	{
		VK_CHECK(vkCreateCommandPool(m_logicalDevice, &commandPoolCreateInfo, nullptr, &m_frames[frame].m_commandPool));
		m_deleter.Push([this, f = frame]() {vkDestroyCommandPool(m_logicalDevice, m_frames[f].m_commandPool, nullptr); });

		VkCommandBufferAllocateInfo commandBufferAllocateInfo = VkInit::CommandBufferAllocateInfo(m_frames[frame].m_commandPool, 1);

		VK_CHECK(vkAllocateCommandBuffers(m_logicalDevice, &commandBufferAllocateInfo, &m_frames[frame].m_commandBuffer));
	}


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

	VkAttachmentDescription depth_attachment = {};
	// Depth attachment
	depth_attachment.flags = 0;
	depth_attachment.format = m_depthFormat;
	depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_attachment_ref = {};
	depth_attachment_ref.attachment = 1;
	depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference color_attachment_ref = {};
	//attachment number will index into the pAttachments array in the parent renderpass itself
	color_attachment_ref.attachment = 0;
	color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//we are going to create 1 subpass, which is the minimum you can do
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &color_attachment_ref;
	subpass.pDepthStencilAttachment = &depth_attachment_ref;

	//array of 2 attachments, one for the color, and other for depth
	std::vector<VkAttachmentDescription> attachments = { colorAttachment, depth_attachment };

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkSubpassDependency depth_dependency = {};
	depth_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	depth_dependency.dstSubpass = 0;
	depth_dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	depth_dependency.srcAccessMask = 0;
	depth_dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	depth_dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	std::vector<VkSubpassDependency> dependencies = { dependency, depth_dependency };

	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	//2 attachments from said array
	render_pass_info.attachmentCount = attachments.size();
	render_pass_info.pAttachments = attachments.data();
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass;
	render_pass_info.dependencyCount = dependencies.size();
	render_pass_info.pDependencies = dependencies.data();

	//connect the color attachment to the info
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
	for (int i = 0; i < swapchain_imagecount; i++) 
	{
		std::vector<VkImageView> attachments = { m_swapChainImages.imageViews[i], m_depthImageView };
		fb_info.attachmentCount = attachments.size();
		fb_info.pAttachments = attachments.data();
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

	for (int frame = 0; frame < FRAMES; frame++)
	{
		VK_CHECK(vkCreateFence(m_logicalDevice, &fenceCreateInfo, nullptr, &m_frames[frame].m_renderFence));
		m_deleter.Push([this, f = frame]() {vkDestroyFence(m_logicalDevice, m_frames[f].m_renderFence, nullptr); });

		//for the semaphores we don't need any flags
		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.flags = 0;

		VK_CHECK(vkCreateSemaphore(m_logicalDevice, &semaphoreCreateInfo, nullptr, &m_frames[frame].m_presentSemaphore));
		m_deleter.Push([this, f = frame]() {vkDestroySemaphore(m_logicalDevice, m_frames[f].m_presentSemaphore, nullptr); });
		VK_CHECK(vkCreateSemaphore(m_logicalDevice, &semaphoreCreateInfo, nullptr, &m_frames[frame].m_renderSemaphore));
		m_deleter.Push([this, f = frame]() {vkDestroySemaphore(m_logicalDevice, m_frames[f].m_renderSemaphore, nullptr); });
	}
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

	//setup push constants
	VkPushConstantRange push_constant;
	//this push constant range starts at the beginning
	push_constant.offset = 0;
	//this push constant range takes up the size of a MeshPushConstants struct
	push_constant.size = sizeof(MeshPushConstant);
	//this push constant range is accessible only in the vertex shader
	push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	pipeline_layout_info.pPushConstantRanges = &push_constant;
	pipeline_layout_info.pushConstantRangeCount = 1;

	pipeline_layout_info.setLayoutCount = 1;
	pipeline_layout_info.pSetLayouts = &m_globalSetLayout;

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
	VertexInputDescription vertexDescription = Vertex::GetVertexInputDescription();
	m_vertexInputInfo.vertexAttributeDescriptionCount = vertexDescription.attributes.size();
	m_vertexInputInfo.pVertexAttributeDescriptions = vertexDescription.attributes.data();

	m_vertexInputInfo.vertexBindingDescriptionCount = vertexDescription.bindings.size();
	m_vertexInputInfo.pVertexBindingDescriptions = vertexDescription.bindings.data();

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
	VK_CHECK(vkWaitForFences(m_logicalDevice, 1, &GetCurrentFrame().m_renderFence, true, 1000000000));
	VK_CHECK(vkResetFences(m_logicalDevice, 1, &GetCurrentFrame().m_renderFence));

	uint32_t swapchainImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(m_logicalDevice, m_swapchain, 1000000000, GetCurrentFrame().m_presentSemaphore, nullptr, &swapchainImageIndex));

	//now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
	VK_CHECK(vkResetCommandBuffer(GetCurrentFrame().m_commandBuffer, 0));

	//naming it cmd for shorter writing
	VkCommandBuffer cmd = GetCurrentFrame().m_commandBuffer;

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

	VkClearValue depthValue;
	depthValue.depthStencil.depth = 1.0f;

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
	std::vector<VkClearValue> clearValues = { clearValue, depthValue };
	rpInfo.clearValueCount = clearValues.size();
	rpInfo.pClearValues = clearValues.data();
	glm::vec3 camPos = { 0.f,0.f,-2.f };

	glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
	glm::mat4 projection = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 200.0f);
	projection[1][1] *= -1;

	//fill a GPU camera data struct
	GPUCameraData camData;
	camData.proj = projection;
	camData.view = view;
	camData.viewproj = projection * view;

	//and copy it to the buffer
	void* data;
	vmaMapMemory(m_allocator, GetCurrentFrame().m_cameraBuffer.allocation, &data);

	memcpy(data, &camData, sizeof(GPUCameraData));

	vmaUnmapMemory(m_allocator, GetCurrentFrame().m_cameraBuffer.allocation);

	glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::radians(m_frameNumber * 0.4f), glm::vec3(0, 1, 0));

	//calculate final mesh matrix
	glm::mat4 mesh_matrix = projection * view * model;

	MeshPushConstant constants;
	constants.renderMatrix = mesh_matrix;

	vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(GetCurrentFrame().m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(cmd, 0, 1, &mesh.m_vertexBuffer.buffer, &offset);
	vkCmdPushConstants(cmd, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstant), &constants);
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &GetCurrentFrame().m_globalDescriptor, 0, nullptr);
	vkCmdDraw(cmd, 3, 1, 0, 0);

	vkCmdEndRenderPass(cmd);

	VK_CHECK(vkEndCommandBuffer(GetCurrentFrame().m_commandBuffer));

	VkSubmitInfo submit = {};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.pNext = nullptr;

	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	submit.pWaitDstStageMask = &waitStage;

	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &GetCurrentFrame().m_presentSemaphore;

	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &GetCurrentFrame().m_renderSemaphore;

	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &cmd;

	//submit command buffer to the queue and execute it.
	// _renderFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit(m_graphicsQueue, 1, &submit, GetCurrentFrame().m_renderFence));

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;

	presentInfo.pSwapchains = &m_swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores = &GetCurrentFrame().m_renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	VK_CHECK(vkQueuePresentKHR(m_graphicsQueue, &presentInfo));

	//increase the number of frames drawn
	m_frameNumber++;
}

void VulkanEngine::LoadMesh()
{
	UploadMesh(mesh);
}

void VulkanEngine::InitDescriptors()
{
	//create a descriptor pool that will hold 10 uniform buffers
	std::vector<VkDescriptorPoolSize> sizes =
	{
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10 }
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = 0;
	pool_info.maxSets = 10;
	pool_info.poolSizeCount = (uint32_t)sizes.size();
	pool_info.pPoolSizes = sizes.data();

	vkCreateDescriptorPool(m_logicalDevice , &pool_info, nullptr, &m_descriptorPool);

	m_deleter.Push([this]() {vkDestroyDescriptorPool(m_logicalDevice, m_descriptorPool, nullptr); });

	//information about the binding.
	VkDescriptorSetLayoutBinding camBufferBinding = {};
	camBufferBinding.binding = 0;
	camBufferBinding.descriptorCount = 1;
	// it's a uniform buffer binding
	camBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	// we use it from the vertex shader
	camBufferBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;


	VkDescriptorSetLayoutCreateInfo setinfo = {};
	setinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	setinfo.pNext = nullptr;

	//we are going to have 1 binding
	setinfo.bindingCount = 1;
	//no flags
	setinfo.flags = 0;
	//point to the camera buffer binding
	setinfo.pBindings = &camBufferBinding;

	vkCreateDescriptorSetLayout(m_logicalDevice, &setinfo, nullptr, &m_globalSetLayout);
	m_deleter.Push([this]() {vkDestroyDescriptorSetLayout(m_logicalDevice, m_globalSetLayout, nullptr); });

	for (int i = 0; i < FRAMES; i++)
	{
		m_frames[i].m_cameraBuffer = CreateBuffer(sizeof(GPUCameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		m_deleter.Push([this, frame = i]() {vmaDestroyBuffer(m_allocator, m_frames[frame].m_cameraBuffer.buffer, m_frames[frame].m_cameraBuffer.allocation); });

		//allocate one descriptor set for each frame
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.pNext = nullptr;
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		//using the pool we just set
		allocInfo.descriptorPool = m_descriptorPool;
		//only 1 descriptor
		allocInfo.descriptorSetCount = 1;
		//using the global data layout
		allocInfo.pSetLayouts = &m_globalSetLayout;

		vkAllocateDescriptorSets(m_logicalDevice, &allocInfo, &m_frames[i].m_globalDescriptor);

		//information about the buffer we want to point at in the descriptor
		VkDescriptorBufferInfo binfo;
		//it will be the camera buffer
		binfo.buffer = m_frames[i].m_cameraBuffer.buffer;
		//at 0 offset
		binfo.offset = 0;
		//of the size of a camera data struct
		binfo.range = sizeof(GPUCameraData);

		VkWriteDescriptorSet setWrite = {};
		setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		setWrite.pNext = nullptr;

		//we are going to write into binding number 0
		setWrite.dstBinding = 0;
		//of the global descriptor
		setWrite.dstSet = m_frames[i].m_globalDescriptor;

		setWrite.descriptorCount = 1;
		//and the type is uniform buffer
		setWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		setWrite.pBufferInfo = &binfo;


		vkUpdateDescriptorSets(m_logicalDevice, 1, &setWrite, 0, nullptr);

	}

}

void VulkanEngine::UploadMesh(Mesh& mesh)
{
	//allocate vertex buffer
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	//this is the total size, in bytes, of the buffer we are allocating
	bufferInfo.size = mesh.m_vertices.size() * sizeof(Vertex);
	//this buffer is going to be used as a Vertex Buffer
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;


	//let the VMA library know that this data should be writeable by CPU, but also readable by GPU
	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

	//allocate the buffer
	VK_CHECK(vmaCreateBuffer(m_allocator, &bufferInfo, &vmaallocInfo,
		&mesh.m_vertexBuffer.buffer,
		&mesh.m_vertexBuffer.allocation,
		nullptr));


	//add the destruction of triangle mesh buffer to the deletion queue
	m_deleter.Push([this, mesh]() {vmaDestroyBuffer(m_allocator, mesh.m_vertexBuffer.buffer, mesh.m_vertexBuffer.allocation); });

	void* data;
	vmaMapMemory(m_allocator, mesh.m_vertexBuffer.allocation, &data);

	memcpy(data, mesh.m_vertices.data(), mesh.m_vertices.size() * sizeof(Vertex));

	vmaUnmapMemory(m_allocator, mesh.m_vertexBuffer.allocation);
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
	m_depthStencil = VkInit::DepthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);
	pipelineInfo.pDepthStencilState = &m_depthStencil;

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

	auto error = SDL_GetError();
	std::cout << error;

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

	m_window = SDL_CreateWindow(
		"Vulkan Engine",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WindowSize.width,
		WindowSize.height,
		window_flags
	);
	error = SDL_GetError();
	std::cout << error;

	InitVulkan();

	InitSwapchain();

	InitCommands();

	InitDefaultRenderPass();

	InitFrameBuffers();

	InitSyncStructures();

	InitDescriptors();

	InitPipelines();

	LoadMesh();
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
	for (int i = 0; i < FRAMES; i++)
	{
		vkWaitForFences(m_logicalDevice, 1, &m_frames[i].m_renderFence, true, 1000000000);
	}

	m_deleter.Flush();

	vmaDestroyAllocator(m_allocator);

	vkDestroyDevice(m_logicalDevice, nullptr);
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkb::destroy_debug_utils_messenger(m_instance, m_debugMessenger);
	vkDestroyInstance(m_instance, nullptr);

	// Clean memory for allocator

	SDL_DestroyWindow(m_window);
}

#pragma region Notes

//Transfer - only queue families in Vulkan are very interesting, as they are used to perform background asset uploadand
//they often can run completely asynchronously from rendering.
//So if you want to upload assets to the GPU from a background thread, 
//using a Transfer - only queue will be a great choice.

#pragma endregion

