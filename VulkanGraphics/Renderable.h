#pragma once
#include "Mesh.h"
#include <vulkan/vulkan.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>

import Helper;

class Renderable
{
private:
	Mesh* m_mesh;
	glm::mat4 m_transformMatrix;
	glm::mat4 m_defaultMatrix;
	float m_alpha;
	int m_texId;
	VkPipeline m_pipeline;
	float m_width;
	float m_height;
	float m_posX;
	float m_posY;
	float m_angle;
	float m_scaleX;
	float m_scaleY;
	Color m_color;
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
	float GetX() { return m_posX; }
	float GetY() { return m_posY; }
	float GetWidth() { return m_width; }
	float GetHeight() { return m_height; }
	float GetAngle() { return m_angle; }
	float GetAlphaRef() { return m_alpha; }
	void SetSize(float x, float y);
	float GetSizeX() { return m_width; }
	float GetSizeY() { return m_height; }
	float GetScaleX() const { return m_scaleX; }
	float GetScaleY() const { return m_scaleY; }
	Color GetColor() const { return m_color; }
	void SetColor(const Color& color) { m_color = color; }
};