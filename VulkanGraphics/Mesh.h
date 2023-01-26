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
	glm::vec3 color;
	glm::vec2 uv;

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
		//make the array 3 vertices long
		m_vertices.resize(6);
		m_vertices[0].position = { 2.5f, 2.5f, 0.0f };
		m_vertices[1].position = { -2.5f, 2.5f, 0.0f };
		m_vertices[2].position = { -2.5f, 0.0f, 0.0f };
		m_vertices[0].uv.x = 1.0f;
		m_vertices[0].uv.y = 1.0f;
		m_vertices[1].uv.x = 0.0f;
		m_vertices[1].uv.y = 1.0f;
		m_vertices[2].uv.x = 0.0f;
		m_vertices[2].uv.y = 0.0f;

		m_vertices[3].position = { -2.5f, 0.0f, 0.0f };
		m_vertices[4].position = { 2.5f, 0.0f, 0.0f };
		m_vertices[5].position = { 2.5f, 2.5f, 0.0f };
		m_vertices[3].uv.x = 0.0f;
		m_vertices[3].uv.y = 0.0f;
		m_vertices[4].uv.x = 1.0f;
		m_vertices[4].uv.y = 0.0f;
		m_vertices[5].uv.x = 1.0f;
		m_vertices[5].uv.y = 1.0f;
	}
};