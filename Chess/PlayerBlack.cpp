#include "PlayerBlack.h"
#include "Knight.h"
#include "Bishop.h"
#include "King.h"
#include "Queen.h"
#include "Rook.h"
#include "Pawn.h"

PlayerBlack::PlayerBlack() : Player('B')
{
	// pawns
	for (uint pawnXindex = 0; pawnXindex < NumberOfPawns; pawnXindex++)
		m_pieces.emplace_back(std::make_shared<Pawn>(Pawn(Pair(pawnXindex, BlackPawnPosition), false)));

	for (uint knightIndex = 0; knightIndex < NumberOfKnights; knightIndex++)
		m_pieces.emplace_back(std::make_shared<Knight>(Knight(Pair(KnightPositions[knightIndex], BlackPawnPosition - 1), false)));

	for (uint bishopIndex = 0; bishopIndex < NumberOfBishops; bishopIndex++)
		m_pieces.emplace_back(std::make_shared<Bishop>(Bishop(Pair(BishopPositions[bishopIndex], BlackPawnPosition - 1), false)));

	for (uint rookIndex = 0; rookIndex < NumberOfRooks; rookIndex++)
		m_pieces.emplace_back(std::make_shared<Rook>(Rook(Pair(RookPositions[rookIndex], BlackPawnPosition - 1), false)));

	m_pieces.emplace_back(std::make_shared<King>(King(Pair(KingPosition, BlackPawnPosition - 1), false)));
	m_pieces.emplace_back(std::make_shared<Queen>(Queen(Pair(QueenPosition, BlackPawnPosition - 1), false)));
}
