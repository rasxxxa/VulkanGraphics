#pragma once
#include "Piece.h"
#include "Table.h"
using namespace std;
class Bishop : public Piece
{
private:
	static std::vector<Pair> directons;
public:
	Bishop(const Pair& _position, bool _isWhite);
	virtual std::vector<Pair> GetPositionToMove() override;
	virtual std::string GetPieceName() override { return "bishop"; };
};

