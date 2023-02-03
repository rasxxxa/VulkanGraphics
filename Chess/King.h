#pragma once
#include "Piece.h"
#include "Table.h"
class King :
    public Piece
{
private:
	static std::vector<Pair> directons;
public:
	King(const Pair& _position, bool _isWhite);
	virtual std::vector<Pair> GetPositionToMove() override;
	virtual std::string GetPieceName() override { return "king"; };
};

