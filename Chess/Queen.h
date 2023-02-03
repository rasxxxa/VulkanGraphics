#pragma once
#include "Piece.h"
#include "Table.h"
class Queen :
    public Piece
{
private:
	static std::vector<Pair> directons;
public:
	Queen(const Pair& _position, bool _isWhite);
	virtual std::vector<Pair> GetPositionToMove() override;
	virtual std::string GetPieceName() override { return "queen"; };
};

