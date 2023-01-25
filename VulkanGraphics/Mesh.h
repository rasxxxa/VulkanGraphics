#pragma once
#include <vulkan/vulkan.h>
#include <glm/glm/vec3.hpp>
#include <vector>
#include "VkTypes.h"

struct VertexInputDescription 
{

	std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;

	VkPipelineVertexInputStateCreateFlags flags = 0;
};


struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;

	static VertexInputDescription GetVertexInputDescription();
};

class Mesh
{
private:
	std::vector<Vertex> m_vertices;
	AllocatedBuffer m_vertexBuffer;
public:
	friend class VulkanEngine;
	Mesh()
	{
		m_vertices.resize(6);
		m_vertices[0].position = { 1.f, 1.f, 0.0f };
		m_vertices[1].position = { -1.f, 1.f, 0.0f };
		m_vertices[2].position = { 0.f,-1.f, 0.0f };

		m_vertices[0].color = { 0.f, 1.f, 0.0f }; //pure green
		m_vertices[1].color = { 0.f, 1.f, 0.0f }; //pure green
		m_vertices[2].color = { 0.f, 1.f, 0.0f }; //pure green
	}
};