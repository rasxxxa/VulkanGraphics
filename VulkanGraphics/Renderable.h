#pragma once
#include "Mesh.h"
#include <vulkan/vulkan.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>
class Renderable
{
private:
	Mesh* m_mesh;
	glm::mat4 m_transformMatrix;
	float m_alpha;
	int m_texId;
	VkPipeline m_pipeline;
	float m_width;
	float m_height;
	float m_posX;
	float m_posY;
	float m_angle;
public:
	Renderable();
	inline float GetAlpha() const { return m_alpha; }
	inline void SetAlpha(float alpha = 1.0f) { this->m_alpha = alpha; }
	inline const int GetTexId() const { return m_texId; }
	inline const VkPipeline GetPipeline() const { return m_pipeline; }
	inline void SetPipeline(const VkPipeline& pipeline) { m_pipeline = pipeline; }
	inline Mesh* GetMesh() { return m_mesh; }
	inline void SetMesh(Mesh* mesh) { this->m_mesh = mesh; }
	void SetPosition(float x, float y);
	inline void SetTexId(int id) { this->m_texId = id; }
	inline glm::mat4 GetTransformMatrix() const { return m_transformMatrix; };
	void SetRotation(float angle);
	float& GetX() { return m_transformMatrix[3].x; }
	float& GetY() { return m_transformMatrix[3].y;; }
	float& GetWidth() { return m_width; }
	float& GetHeight() { return m_height; }
	float& GetAngle() { return m_angle; }
};