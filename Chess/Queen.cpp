#include "Queen.h"
Queen::Queen(const Pair& _position, bool _isWhite) : Piece(_position, 'Q', _isWhite, GetPieceName())
{
}

std::vector<Pair> Queen::GetPositionToMove()
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

std::vector<Pair> Queen::directons =
{
	{1 , 1}, // up right
	{1 , -1}, // down right
	{-1 , 1}, // down left
	{-1 , -1}, // up left
    {1 , 0}, // right
	{0 , 1}, // down
	{-1 , 0}, // left
	{0 , -1}, // up
};