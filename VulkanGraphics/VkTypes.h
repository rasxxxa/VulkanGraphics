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
	glm::vec4 data;
	glm::mat4 renderMatrix;
};