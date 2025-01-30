#pragma once
#include "Particle.hpp"
#include "Spoon.hpp"

class PhysicsSolver
{
public:

	std::vector<Particle> particles;

	sf::Vector2f gravity = sf::Vector2f(0, 9.81);
	bool gravityON = false;

	float DT = 0.1;

	float const particleSize = 5;

	float initialVelocityX = 50;
	float initialVelocityY = 50;

	PhysicsSolver(int resolutionX, int resolutionY, int s)
	{
		boundX = resolutionX;
		boundY = resolutionY;

		substeps = s;

		subDT = DT / (float)substeps;

		particles.reserve(1000);

		initialVelocityX /= subDT;
		initialVelocityY /= subDT;
	}

	void AddParticle()
	{
		particles.emplace_back(sf::Vector2f(RandomNumber(0, boundX), RandomNumber(0, boundY)), sf::Vector2f(RandomNumber(-initialVelocityX, initialVelocityX), RandomNumber(-initialVelocityY, initialVelocityY)), particleSize, nextID);
		nextID++;
	}

	void PhysicsUpdate()
	{
		for (int step = 0; step < substeps; step++)
		{
			for (int i = 0; i < particles.size(); i++)
			{
				if (gravityON) particles.at(i).acceleration += gravity;

				EdgeCheck(i);

				particles.at(i).Update(subDT);

				if (!particles.at(i).atRest)
				{


					ParticleCollision(i);
				}
			}
		}
	}

	void GravitySwitch()
	{
		gravityON = !gravityON;
	}

private:

	int nextID = 0;

	float absorption = 1.f;
	float drag = 1.f;

	int boundX;
	int boundY;


	int substeps;
	float subDT;

	void EdgeCheck(int const &index)
	{
		sf::Vector2f dx = { -particles.at(index).GetVelocity().x, particles.at(index).GetVelocity().y};
		sf::Vector2f dy = { particles.at(index).GetVelocity().x, -particles.at(index).GetVelocity().y };

		if (particles.at(index).position.x + particles.at(index).size * 2 > boundX || particles.at(index).position.x + particles.at(index).size < 0)
		{
			if (particles.at(index).position.x + particles.at(index).size * 2 > boundX)
			{
				particles.at(index).position.x = boundX - particles.at(index).size * 2;
			}
			if (particles.at(index).position.x + particles.at(index).size < 0)
			{
				particles.at(index).position.x = 0;
			}
			particles.at(index).SetVelocity(dx * absorption);
		}
		if (particles.at(index).position.y + particles.at(index).size * 2 > boundY || particles.at(index).position.y + particles.at(index).size < 0)
		{
			if (particles.at(index).position.y + particles.at(index).size * 2 > boundY)
			{
				particles.at(index).position.y = boundY - particles.at(index).size * 2;
			}
			if (particles.at(index).position.y + particles.at(index).size < 0)
			{
				particles.at(index).position.y = 0;
			}
			particles.at(index).SetVelocity(dy * absorption);
		}
	}

	sf::Vector2f ObjectCollision(sf::Vector2f const &v, sf::Vector2f const &n) 
	{
		return (2 * (v.x * n.x + v.y * n.y) * n - v ) * absorption;
	}

	void ParticleCollision(int &index)
	{

	}
};