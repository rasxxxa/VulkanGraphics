#include "Pair.h"

Pair::Pair(const short& _x, const short& _y) : x(_x), y(_y)
{

}



Pair::Pair()
{
	x = 0;
	y = 0;
}



Pair::Pair(const std::initializer_list<short>& elements)
{
	if (elements.size() < 2)
	{
		x = 0;
		y = 0;
	}
	else
	{
		x = *elements.begin();
		y = *(elements.begin() + 1);
	}
}

