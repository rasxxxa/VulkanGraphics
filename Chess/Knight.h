#pragma once
#include "Piece.h"
#include "Table.h"
using namespace constants;

class Knight : public Piece
{
private:
	static std::vector<Pair> waysToMove;
public:
	Knight(const Pair& _position, bool _isWhite);
	virtual std::vector<Pair> GetPositionToMove() override;
	virtual std::string GetPieceName() override { return "knight"; };
};

