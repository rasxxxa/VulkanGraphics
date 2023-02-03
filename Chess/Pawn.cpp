#include "Pawn.h"
#include "Table.h"

Pawn::Pawn(const Pair& _position, bool _isWhite) : Piece(_position, 'P', _isWhite, GetPieceName())
{

}

std::vector<Pair> Pawn::GetPositionToMove()
{
    std::vector<Pair> moves;

    char step = m_isWhite ? 1 : -1;
    char doubleStep = m_isWhite ? 2 : -2;

    if (m_isStartingPosition)
    {
        if (!Table::elements[m_position.x][m_position.y + step] && !Table::elements[m_position.x][m_position.y + doubleStep])
            moves.emplace_back(m_position.x, m_position.y + doubleStep);
    }

    if (m_position.y + step < TableSize - 1 && m_position.y + step > 0)
        if (!Table::elements[m_position.x][m_position.y + step])
            moves.emplace_back(m_position.x, m_position.y + step);

    // TODO: an poisant

    return moves;
}
