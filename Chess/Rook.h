#pragma once
#include "Piece.h"
class Rook :
    public Piece
{
private:
	static std::vector<Pair> directons;
public:
	Rook(const Pair& _position, bool _isWhite);
	virtual std::vector<Pair> GetPositionToMove() override;
	virtual std::string GetPieceName() override { return "rook"; };
};

