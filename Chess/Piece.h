#pragma once
#include "Pair.h"
#include <iostream>
#include <vector>
#include "../VulkanGraphics/VulkanEngine.h"
class Piece
{
protected:
	Pair m_position;
	char m_figureCode;
	bool m_isWhite;
	bool m_isStartingPosition;
	std::string m_stringSufix;
	DrawObject m_pieceRenderable;
	virtual std::ostream& print(std::ostream& out) const;
public:
	Piece(const Pair& _position, const char& _figureCode, bool _isWhite, const std::string pieceName);
	virtual ~Piece();
	void SetPosition(const Pair& _position);
	const Pair& GetPosition() const;
	virtual std::vector<Pair> GetPositionToMove() = 0;
	
	friend std::ostream& operator<<(std::ostream& os, const Piece& piece);
	virtual std::string GetPieceName() = 0;
};

