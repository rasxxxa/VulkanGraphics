#pragma once
#include <SDL.h>

#include <iostream>
#include <vulkan/vulkan.h>
#include <vector>
#include "vma/vk_mem_alloc.h"
#include "vkbootstrap/VkBootstrap.h"
#include "DeletionQueue.h"
#include "Mesh.h"
#include <unordered_map>
#include "../Random/Random.h"
#include "../imgui/imconfig.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_vulkan.h"
#include "../imgui/imgui_impl_sdl.h"
#include "Renderable.h"
import Helper;

using DrawObject = Renderable*;

class VulkanEngine
{
private:
	
	struct CameraPos
	{
		float x;
		float y;
		float z;
		CameraPos() : x(-1.0f), y(-1.0f), z(-1.0f)
		{

		}
	};

	struct Perspective
	{
		float fovy;
		float ratio;
		float znear;
		float zfar;
		Perspective() : fovy(90.0f), ratio(1.0f), znear(-0.0f), zfar(200.0f)
		{

		}
	};

	CameraPos m_mainCamera;
	Perspective m_mainPerspective;
	std::vector<VkDescriptorSet> m_samplersDescriptorSets;

	struct Texture 
	{
		AllocatedImage image;
		VkImageView imageView;
	};

	std::unordered_map<VkPipeline, VkPipelineLayout> m_pipelineMAPlayouts;

	std::vector<Renderable> m_renderables;

	VkDescriptorSetLayout m_globalSetLayout;
	VkDescriptorSetLayout m_objectSetLayout;
	VkDescriptorPool m_descriptorPool;

	struct Frame
	{
		VkSemaphore m_presentSemaphore, m_renderSemaphore;
		VkFence m_renderFence;

		VkCommandPool m_commandPool;
		VkCommandBuffer m_commandBuffer;

		AllocatedBuffer m_cameraBuffer;
		VkDescriptorSet m_globalDescriptor;

		AllocatedBuffer m_objectBuffer;
		VkDescriptorSet m_objectDescriptor;
	};

	enum class TypeOfObject {Simple, Texture};

	struct UploadContext 
	{
		VkFence m_uploadFence;
		VkCommandPool m_commandPool;
		VkCommandBuffer m_commandBuffer;
	}m_uploadContext;

	Frame& GetCurrentFrame();

	std::vector<Frame> m_frames;

	struct SDL_Window* m_window{ nullptr };
	VkPhysicalDeviceProperties m_GPUProperties;

#pragma region DepthImage
	VkImageView m_depthImageView;
	AllocatedImage m_depthImage;
	VkFormat m_depthFormat;
#pragma endregion

#pragma region MESHES
	std::unordered_map<std::string, Mesh> m_meshes;
#pragma endregion

#pragma region Deletor
	DeletionQueue m_deleter;
public:
	inline DeletionQueue& GetDeleter() { return m_deleter; };
private:
#pragma endregion

#pragma region Allocator
	VmaAllocator m_allocator;
	
public:
	inline VmaAllocator GetAllocator() { return m_allocator; };
	AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlagBits usage, VmaMemoryUsage memoryUsage);
private:
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

	VkRenderPass m_renderPass;
	std::vector<VkFramebuffer> m_frameBuffers;

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
	void InitDescriptors();
	VkSampler m_sampler;
	int CreateTextureDescriptor(VkImageView textureImage, VkSampler sampler);
	void DrawObjects(VkCommandBuffer cmd);

	void LoadImage(const std::string& path, const std::string& name);

	void UploadMesh(Mesh& mesh);
	size_t PadUniformBufferSize(size_t originalSize);
	void LoadImgGui();
	//texture hashmap
	std::unordered_map<std::string, Texture> m_loadedTextures;
	std::unordered_map<std::string, int> m_texturesAndIDS;

public:
	void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);
private:
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
	VkPipeline BuildPipeline(VkDevice device, VkRenderPass pass, VkPipelineLayout pipelineLayout);
	VkPipelineLayout m_texturePipelineLayout, m_simpleObjectPipelineLayout;
	VkPipeline m_texturePipeline, m_simpleObjectPipeline;
	VkPipelineDepthStencilStateCreateInfo m_depthStencil;
#pragma endregion

#pragma region Textures
	VkDescriptorPool m_samplerDescriptorPool;
	VkDescriptorSetLayout m_singleTextureSetLayout;
#pragma endregion

	VulkanEngine();
public:
	static VulkanEngine& Get();
	VulkanEngine(const VulkanEngine&) = delete;
	VulkanEngine(const VulkanEngine&&) = delete;
	Renderable* CreateObject();
	Renderable* CreateObject(const std::string& texturePath);
	void Init();
	void Run();
	void CleanUp();
};