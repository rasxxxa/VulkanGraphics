#include "Renderable.h"

Renderable::Renderable() : m_alpha(1.0), m_texId(-1), m_transformMatrix(glm::mat4(0.0f)), m_mesh(nullptr), m_pipeline(nullptr)
, m_width(0.2f), m_height(0.2f), m_posX(0.0f), m_posY(0.0f), m_angle(0.0f)
{
	m_transformMatrix = glm::translate(glm::vec3{ 0, 0, 0 });
}

void Renderable::SetPosition(float x, float y)
{
	m_transformMatrix[3].x = x;
	m_transformMatrix[3].y = y;
	m_posX = x;
	m_posY = y;
}

void Renderable::SetRotation(float angle)
{
//	m_transformMatrix = glm::rotate(m_transformMatrix, glm::radians(360-m_angle), glm::vec3(0.0, 0.0, 1));
//	m_angle += angle;
	m_transformMatrix = glm::rotate(m_transformMatrix, glm::radians(m_angle), glm::vec3(0.0 , 0.0 , 1));
}
