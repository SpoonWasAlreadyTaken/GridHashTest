#pragma once
#include "FaultyUtilities.hpp"

class Particle
{
public:
	sf::Vector2f position;
	sf::Vector2f lastPosition;
	sf::Vector2f acceleration;

	float size;

	Particle(sf::Vector2f pos, sf::Vector2f a, float s)
	{
		position = pos;
		lastPosition = position;
		acceleration = a;
		size = s;
	}

	void SetVelocity(sf::Vector2f v) { lastPosition = position - v; }
	void AddVelocity(sf::Vector2f v) { lastPosition = GetVelocity() + v; }
	void Accelerate(sf::Vector2f a) { acceleration += a; }
	void Stop() { lastPosition = position; }
	sf::Vector2f&& GetVelocity() const  { return position - lastPosition; }


	void Update(float const DT)
	{
		sf::Vector2f displacement = position - lastPosition;
		lastPosition = position;
		position += displacement + acceleration * (DT * DT);
		acceleration = {};
	}
};