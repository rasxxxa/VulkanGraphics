#include "PlayerWhite.h"
#include "Knight.h"
#include "Bishop.h"
#include "King.h"
#include "Queen.h"
#include "Rook.h"
#include "Pawn.h"

PlayerWhite::PlayerWhite() : Player('W')
{
	// pawns
	for (uint pawnXindex = 0; pawnXindex < NumberOfPawns; pawnXindex++)
		m_pieces.emplace_back(std::make_shared<Pawn>(Pawn(Pair(pawnXindex, WhitePawnPosition), true)));

	for (uint knightIndex = 0; knightIndex < NumberOfKnights; knightIndex++)
		m_pieces.emplace_back(std::make_shared<Knight>(Knight(Pair(KnightPositions[knightIndex], WhitePawnPosition + 1), true)));

	for (uint bishopIndex = 0; bishopIndex < NumberOfBishops; bishopIndex++)
		m_pieces.emplace_back(std::make_shared<Bishop>(Bishop(Pair(BishopPositions[bishopIndex], WhitePawnPosition + 1), true)));

	for (uint rookIndex = 0; rookIndex < NumberOfRooks; rookIndex++)
		m_pieces.emplace_back(std::make_shared<Rook>(Rook(Pair(RookPositions[rookIndex], WhitePawnPosition + 1), true)));

	m_pieces.emplace_back(std::make_shared<King>(King(Pair(KingPosition, WhitePawnPosition + 1), true)));
	m_pieces.emplace_back(std::make_shared<Queen>(Queen(Pair(QueenPosition, WhitePawnPosition + 1), true)));
}