#pragma once
#include "Piece.h"

class Pawn :
    public Piece
{
public:
    Pawn(const Pair& _position, bool _isWhite);
    virtual std::vector<Pair> GetPositionToMove() override;
    virtual std::string GetPieceName() override { return "pawn"; };
};

