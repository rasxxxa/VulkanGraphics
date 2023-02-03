#include "Player.h"

Player::Player(const char& playerMark) : m_playerMark(playerMark)
{

}

const std::vector<std::shared_ptr<Piece>>& Player::GetPieces() const
{
	return m_pieces;
}
