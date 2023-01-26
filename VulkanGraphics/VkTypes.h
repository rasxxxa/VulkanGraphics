#pragma once
#include <vma/vk_mem_alloc.h>
#include <glm/glm/glm.hpp>
#include <vulkan/vulkan.h>

struct AllocatedBuffer
{
	VkBuffer buffer;
	VmaAllocation allocation;
};

struct MeshPushConstant
{
	glm::mat4 renderMatrix;
	float hasTexture;
	float alpha;
};

struct AllocatedImage
{
	VkImage image;
	VmaAllocation allocation;
};

struct GPUCameraData {
	glm::mat4 proj;
	glm::mat4 view;
};
