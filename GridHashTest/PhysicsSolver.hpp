#pragma once
#include "Particle.hpp"
#include "SpatialHashing.hpp"
#include "FaultyUtilities.hpp"

class PhysicsSolver
{
public:

	std::vector<Particle> particles;
	SpatialHashing spatialHashing;

	sf::Vector2f gravity = sf::Vector2f(0, 9.81);
	bool gravityON = false;

	float DT = 0.01;

	float spawnLocX;
	float spawnLocY;

	float particleSize;

	float initialVelocityX = 50;
	float initialVelocityY = 50;

	PhysicsSolver(int resolutionX, int resolutionY, int s, float size)
	{
		boundX = resolutionX;
		boundY = resolutionY;
		particleSize = size;

		substeps = s;

		spawnLocX = boundX;
		spawnLocY = boundY - size * 2;

		subDT = DT / (float)substeps;

		particles.reserve(5000);

		initialVelocityX /= subDT;
		initialVelocityY /= subDT;

		SetSpatialHashing();
	}

	void SetSpatialHashing()
	{
		spatialHashing.cellSize = ceilf((particleSize) * 2);

		spatialHashing.columsX = boundX / spatialHashing.cellSize;
		spatialHashing.rowsY = boundY / spatialHashing.cellSize;

		spatialHashing.gridCount = spatialHashing.columsX * spatialHashing.rowsY;

		spatialHashing.grid = new std::vector<int> * [spatialHashing.rowsY];
		for (int i = 0; i < spatialHashing.rowsY; i++)
		{
			spatialHashing.grid[i] = new std::vector<int>[spatialHashing.columsX];
		}
		for (int i = 0; i < spatialHashing.rowsY; i++)
		{
			for (int j = 0; j < spatialHashing.columsX; j++)
			{
				spatialHashing.grid[i][j].reserve(5);
			}
		}

		std::cout << "GridCount: " << spatialHashing.gridCount << "\n";
	}

	void AddParticle()
	{
		spawnLocX -= particleSize *2;
		if (spawnLocX < 0)
		{
			spawnLocY -= particleSize * 2;
			spawnLocX = boundX - particleSize * 2;
		}

		//particles.emplace_back(sf::Vector2f(spawnLocX,spawnLocY), sf::Vector2f(0, 0), particleSize, nextID);
		particles.emplace_back(sf::Vector2f(RandomNumber(0, boundX), RandomNumber(0, boundY)), sf::Vector2f(RandomNumber(-initialVelocityX, initialVelocityX), RandomNumber(-initialVelocityY, initialVelocityY)), particleSize, nextID);
		nextID++;
	}

	void PhysicsUpdate()
	{
		for (int step = 0; step < substeps; step++)
		{
			

			for (int i = 0; i < particles.size(); i++)
			{
				if (gravityON) particles[i].acceleration += gravity / DT;

				EdgeCheck(i);

				particles[i].Update(subDT);
					
			}

			FillGrid();

			for (int y = 0; y < spatialHashing.rowsY; y++)
			{
				for (int x = 0; x < spatialHashing.columsX; x++)
				{
					if (!spatialHashing.grid[y][x].empty())
					{
						CheckGrid(y, x);
					}
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

	float absorption = 0.75f;
	float drag = 1.f;

	int boundX;
	int boundY;

	int checkX[5] = {0, 1, -1, 0, 1};
	int checkY[5] = {0, 0, 1, 1, 1};


	int substeps;
	float subDT;

	void EdgeCheck(int const &index)
	{
		sf::Vector2f dx = { -particles[index].GetVelocity().x, particles[index].GetVelocity().y};
		sf::Vector2f dy = { particles[index].GetVelocity().x, -particles[index].GetVelocity().y };

		if (particles[index].position.x + particles[index].size * 2 > boundX || particles[index].position.x < 0)
		{
			if (particles[index].position.x + particles[index].size * 2 > boundX)
			{
				particles[index].position.x = boundX - particles[index].size * 2;
			}
			if (particles[index].position.x < 0)
			{
				particles[index].position.x = 0;
			}
			particles[index].SetVelocity(dx * absorption);
		}
		if (particles[index].position.y + particles[index].size * 2 > boundY || particles[index].position.y< 0)
		{
			if (particles[index].position.y + particles[index].size * 2 > boundY)
			{
				particles[index].position.y = boundY - particles[index].size * 2;
			}
			if (particles[index].position.y < 0)
			{
				particles[index].position.y = 0;
			}
			particles[index].SetVelocity(dy * absorption);
		}
	}

	sf::Vector2f ObjectCollision(sf::Vector2f const &v, sf::Vector2f const &n) 
	{
		return (2 * (v.x * n.x + v.y * n.y) * n - v ) * absorption;
	}

	
	void ParticleCollision(int sY, int sX, int tY, int tX)
	{
		for (int s = 0; s < spatialHashing.grid[sY][sX].size(); s++)
		{
			for (int t = 0; t < spatialHashing.grid[tY][tX].size(); t++)
			{
				int o = spatialHashing.grid[sY][sX][s];
				int i = spatialHashing.grid[tY][tX][t];

				if (spatialHashing.grid[sY][sX][s] == spatialHashing.grid[tY][tX][t])
				{
					continue;
				}

				float distance = sqrt(pow((particles[o].position.x - particles[i].position.x), 2) + pow((particles[o].position.y - particles[i].position.y), 2));

				if (distance < particles[o].size + particles[i].size)
				{
					sf::Vector2f change = ((particles[o].position - particles[i].position) / distance) * 0.25f * (particles[o].size + particles[i].size - distance);
					particles[o].position += change;
					particles[i].position -= change;
					//std::cout << "Particles Collided ID: " << o << " and " << i << " Grid From: X: " << sX << "|Y: " << sY << " Grid To: X:" << tX << "|Y: " << tY << " Change: " << change.x << "|" << change.y << "\n";
				}
			}
		}
	}
	

	void CheckGrid(int gridY, int gridX)
	{
		int newX;
		int newY;

		for (int i = 0; i < 5; i++)
		{
			newY = gridY + checkY[i];
			newX = gridX + checkX[i];

			if (newX < 0 || newY < 0 || newX >= spatialHashing.columsX || newY >= spatialHashing.rowsY)
			{
				continue;
			}

			if (!spatialHashing.grid[newY][newX].empty())
			{
				ParticleCollision(gridY, gridX, newY, newX);
			}
		}
	}

	void FillGrid()
	{
		spatialHashing.ClearGrid();

		int x;
		int y;

		for (int i = 0; i < particles.size(); i++)
		{
			x = particles[i].position.x / spatialHashing.cellSize;
			y = particles[i].position.y / spatialHashing.cellSize;

			if (x < 0 || x >= spatialHashing.columsX) continue;
			if (y < 0 || y >= spatialHashing.rowsY) continue;


			spatialHashing.grid[y][x].emplace_back(i);
		}
	}
};