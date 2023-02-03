#include "Knight.h"

Knight::Knight(const Pair& _position, bool _isWhite) : Piece(_position, 'k', _isWhite, GetPieceName())
{

}

std::vector<Pair> Knight::GetPositionToMove()
{
    std::vector<Pair> moves;

    for (const auto& ways : waysToMove)
    {
        if (m_position.x + ways.x >= 0
            && m_position.x + ways.x <= TableSize - 1
            && m_position.y + ways.y >= 0
            && m_position.y + ways.y <= TableSize - 1)
        {
            if (!Table::elements[m_position.x + ways.x][m_position.y + ways.y])
                moves.emplace_back(m_position.x + ways.x, m_position.y + ways.y);
        }
    }

    return moves;
}

std::vector<Pair> Knight::waysToMove =
{
    {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
    {1, 2}, {1, -2}, {-1, 2}, {-1, -2},
};