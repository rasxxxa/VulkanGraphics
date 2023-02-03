#include "Piece.h"

std::ostream& Piece::print(std::ostream& out) const
{
	out << m_figureCode << (m_isWhite ? ('w') : ('b'));
	return out;
}

Piece::Piece(const Pair& _position, const char& _figureCode, bool _isWhite, const std::string pieceName) : m_position(_position), m_figureCode(_figureCode), m_isWhite(_isWhite)
{
    m_stringSufix = pieceName;
    std::string color = (_isWhite) ? "white" : "black";
    color.append(m_stringSufix);
    color.append(".png");
    std::string path = "../../../../Chess/assets/";
    path.append(color);
    m_pieceRenderable = VulkanEngine::Get().CreateObject(path);
    float width = 2.0f / 8.0f;
    auto sizeX = width / m_pieceRenderable->GetWidth();
    auto sizeY = width / m_pieceRenderable->GetHeight();
    m_pieceRenderable->SetSize(sizeX, sizeY);
    m_pieceRenderable->SetPosition(m_position.x * width, m_position.y * width);
}

Piece::~Piece()
{
}

void Piece::SetPosition(const Pair& _position)
{
	m_position = _position;
}

const Pair& Piece::GetPosition() const
{
	return m_position;
}

std::ostream& operator<<(std::ostream& os, const Piece& piece)
{
	return piece.print(os);
}
