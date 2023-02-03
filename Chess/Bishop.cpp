#include "Bishop.h"


Bishop::Bishop(const Pair& _position, bool _isWhite) : Piece(_position, 'B', _isWhite, GetPieceName())
{
}

std::vector<Pair> Bishop::GetPositionToMove()
{
	std::vector<Pair> moves;
	for (const auto& direction : directons)
	{
		auto startX = m_position.x;
		auto startY = m_position.y;
		while (startX + direction.x >= 0
			&& startX + direction.x <= TableSize - 1
			&& startY + direction.y >= 0
			&& startY + direction.y <= TableSize - 1
			&& !Table::elements[startX + direction.x][startY + direction.y]
			)
		{
			moves.emplace_back(startX + direction.x, startY + direction.y);
			startX += direction.x;
			startY += direction.y;
		};
	}
	return moves;
}


std::vector<Pair> Bishop::directons =
{
	{1 , 1}, // up right
	{1 , -1}, // down right
	{-1 , 1}, // down left
	{-1 , -1}, // up left
};
