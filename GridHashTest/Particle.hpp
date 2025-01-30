#pragma once
#include "Spoon.hpp"

class Particle
{
public:
	sf::Vector2f position;
	sf::Vector2f lastPosition;
	sf::Vector2f acceleration;

	int ID;
	int gridID;

	bool atRest = false;

	float size;

	sf::Color color;

	template <typename V, typename F, typename I>
	Particle(V&& pos, V&& a, F&& s, I&& i)
	{
		position = std::forward<V>(pos);
		lastPosition = position;
		acceleration = std::forward<V>(a);

		size = std::forward<F>(s);

		ID = std::forward<I>(i);


		float r = sin((float)ID * 0.1);
		float g = sin((float)ID * 0.1 * 0.33 * 2 * acos(0));
		float b = sin((float)ID * 0.1 * 0.66 * 2 * acos(0));
		color = sf::Color((255 * r * r), ( 255 * g * g), (255 * b * b));
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

		if (position == lastPosition)
		{
			atRest = true;
		}
		else
		{
			atRest = false;
		}
	}
};