#include "Renderable.h"

Renderable::Renderable() : m_alpha(1.0), m_texId(-1), m_transformMatrix(glm::mat4(0.0f)), m_mesh(nullptr), m_pipeline(nullptr)
, m_width(0.2f), m_height(0.2f), m_posX(0.0f), m_posY(0.0f), m_angle(0.0f), m_scaleX(1.0f), m_scaleY(1.0f)
{
	m_transformMatrix = glm::translate(glm::vec3{ 0, 0, 0 });
	m_defaultMatrix = m_transformMatrix;
	m_color.r = 1.0f;
	m_color.g = 1.0f;
	m_color.b = 1.0f;
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
	m_transformMatrix = m_defaultMatrix;
	m_transformMatrix[3].x = m_posX;
	m_transformMatrix[3].y = m_posY;
	m_angle = angle;
	m_transformMatrix = glm::rotate(m_transformMatrix, glm::radians(m_angle), glm::vec3(0.0 , 0.0 , 1));
	m_transformMatrix = glm::scale(m_transformMatrix, glm::vec3(m_scaleX, m_scaleY, 1));
}

void Renderable::SetSize(float x, float y)
{
	m_transformMatrix = m_defaultMatrix;
	m_transformMatrix[3].x = m_posX;
	m_transformMatrix[3].y = m_posY;
	m_scaleX = x;
	m_scaleY = y;
	m_width *= x;
	m_height *= y;
	m_transformMatrix = glm::rotate(m_transformMatrix, glm::radians(m_angle), glm::vec3(0.0, 0.0, 1));
	m_transformMatrix = glm::scale(m_transformMatrix, glm::vec3(x, y, 1));
}
