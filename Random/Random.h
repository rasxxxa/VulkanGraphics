#pragma once
#include <random>
template <typename T>
class Random
{
private:
	std::mt19937 engine;
	std::random_device device{};
public:
	Random();
	T Get(T min, T max)
	{
		if constexpr (std::is_integral<T>::value)
		{
			std::uniform_int_distribution<T> distribution(min, max);
			return distribution(engine);
		}
		else
		{
			std::uniform_real_distribution<T> distribution(min, max);
			return distribution(engine);
		}
	}
};

template<typename T>
inline Random<T>::Random()
{
	engine = std::mt19937(device());
}
