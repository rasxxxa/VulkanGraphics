#pragma once

#include "Piece.h"
#include <vector>
#include <memory>
class Player
{
protected:
	std::vector<std::shared_ptr<Piece>> m_pieces;
	char m_playerMark;
public:
	Player(const char& playerMark);
	const std::vector<std::shared_ptr<Piece>>& GetPieces() const;
};

