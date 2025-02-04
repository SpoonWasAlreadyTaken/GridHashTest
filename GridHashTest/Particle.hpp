#pragma once
#include "FaultyUtilities.hpp"

class Particle
{
public:
	sf::Vector2f position;
	sf::Vector2f lastPosition;
	sf::Vector2f acceleration;

	int ID;

	float size;

	template <typename V, typename F, typename I>
	Particle(V&& pos, V&& a, F&& s, I&& i)
	{
		position = std::forward<V>(pos);
		lastPosition = position;
		acceleration = std::forward<V>(a);

		size = std::forward<F>(s);

		ID = std::forward<I>(i);
	}

	template <typename V>
	void SetVelocity(V&& v)
	{
		lastPosition = position - v;
	}
	sf::Vector2f&& GetVelocity()
	{
		return position - lastPosition;
	}

	void Update(float const &DT)
	{
		sf::Vector2f displacement = position - lastPosition;
		lastPosition = position;
		position += displacement + acceleration * (DT * DT);
		acceleration = {};
	}
};