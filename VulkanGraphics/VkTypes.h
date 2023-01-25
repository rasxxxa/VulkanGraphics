#pragma once
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

struct AllocatedBuffer
{
	VkBuffer buffer;
	VmaAllocation allocation;
};