#pragma once
#include <array>
namespace constants
{
	typedef unsigned short ushort;
	typedef unsigned int uint;
	typedef unsigned char uchar;

	constexpr ushort TableSize = 8;
	constexpr ushort NumberOfPawns = 8;
	constexpr ushort NumberOfRooks = 2;
	constexpr std::array<ushort, NumberOfRooks> RookPositions = { 0, 7 };
	constexpr ushort NumberOfBishops = 2;
	constexpr std::array<ushort, NumberOfBishops> BishopPositions = { 2, 5 };
	constexpr ushort NumberOfKnights = 2;
	constexpr std::array<ushort, NumberOfKnights> KnightPositions = { 1, 6 };
	constexpr ushort NumberOfKings = 1;
	constexpr ushort NumberOfQueens = 1;
	constexpr ushort WhitePawnPosition = 6;
	constexpr ushort BlackPawnPosition = 1;
	constexpr ushort KingPosition = 3;
	constexpr ushort QueenPosition = 4;
}

class Chess
{
};

