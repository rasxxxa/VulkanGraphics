#pragma once
#include <initializer_list>
#include "Chess.h"
using namespace constants;
class Pair
{
public:
	short x;
	short y;

	Pair(const short& _x, const short& _y);

	Pair();

	Pair(const std::initializer_list<short>& elements);
};

