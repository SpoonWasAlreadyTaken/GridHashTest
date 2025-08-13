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

	float gravityMultiplier = 1.f;
	bool gravityON = false;

	float DT = 0.01;

	float spawnLocX;
	float spawnLocY;

	float particleSize;
	float minCollision;
	float particleDiameter;

	float initialVelocityX = 50;
	float initialVelocityY = 50;

	PhysicsSolver(int resolutionX, int resolutionY, int s, float size)
	{
		boundX = resolutionX;
		boundY = resolutionY;
		particleSize = size;
		particleDiameter = particleSize * 2;
		minCollision = particleDiameter * particleDiameter;

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

		std::cout << "Colums X: " << spatialHashing.columsX << " Rows Y: " << spatialHashing.rowsY << "\n";

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

		particles.emplace_back(sf::Vector2f(spawnLocX,spawnLocY), sf::Vector2f(0, 0), particleSize);
		//particles.emplace_back(sf::Vector2f(RandomNumber(0, boundX), RandomNumber(0, boundY)), sf::Vector2f(RandomNumber(-initialVelocityX, initialVelocityX), RandomNumber(-initialVelocityY, initialVelocityY)), particleSize);
	}


	void PhysicsUpdate()
	{


		uint32_t count = particles.size();
		uint32_t span = count / mt.ActiveThreads();
		uint32_t leftOver = count - (span * mt.ActiveThreads());

		uint32_t gCount = spatialHashing.rowsY;
		uint32_t gSpan = gCount / mt.ActiveThreads();
		uint32_t gLeftOver = gCount - (gSpan * mt.ActiveThreads());

		for (int step = 0; step < substeps; step++)
		{
			t1 = std::chrono::high_resolution_clock::now();
			for (uint8_t i = 0; i < mt.ActiveThreads(); i++)
			{
				mt.AddTask([this, span, leftOver, i]() {UpdateRange(i * span, span, leftOver * (i == mt.ActiveThreads() - 1)); });
			}
			
			mt.WaitForComplete();
			t2 = std::chrono::high_resolution_clock::now();
			
			
			for (uint8_t i = 0; i < mt.ActiveThreads(); i++)
			{
				mt.AddTask([this, gSpan, gLeftOver, i]() {ClearRange(i * gSpan, gSpan, gLeftOver * (i == mt.ActiveThreads() - 1)); });
			}

			mt.WaitForComplete();
			
			

			//spatialHashing.ClearGrid();
			
			FillGrid();
			
			
			/*
			for (uint8_t i = 0; i < mt.ActiveThreads(); i++)
			{
				mt.AddTask([this, span, leftOver, i]() {FillRange(i * span, span, leftOver * (i == mt.ActiveThreads() - 1)); });
				//FillRange(i * span, span, leftOver * (i == mt.ActiveThreads() - 1));
			}

			mt.WaitForComplete();
			*/


			
			for (uint8_t i = 0; i < mt.ActiveThreads(); i++)
			{
				mt.AddTask([this, gSpan, gLeftOver, i]() {DoSlice(i * gSpan, gSpan, gLeftOver * (i == mt.ActiveThreads() - 1)); });
			}

			mt.WaitForComplete();
		}
		
		singleMS = duration_cast<std::chrono::microseconds>(t2 - t1);
		//std::cout << "Physics Udate Time: " << singleMS.count() << "\n";
	}


	// utility functions

	void GravitySwitch()
	{
		gravityON = !gravityON;
	}

	void Force(sf::Vector2f pos, float strength)
	{
		int x = pos.x / spatialHashing.cellSize;
		int y = pos.y / spatialHashing.cellSize;
		int reach = 150 / spatialHashing.cellSize;

		if (x < 0 || x >= spatialHashing.columsX) return;
		if (y < 0 || y >= spatialHashing.rowsY) return;


		for (int i = y - reach; i < y + reach; i++)
		{
			for (int j = x - reach; j < x + reach; j++)
			{
				if (j < 0 || j >= spatialHashing.columsX) continue;
				if (i < 0 || i >= spatialHashing.rowsY) continue;
				for (int p = 0; p < spatialHashing.grid[i][j].size(); p++)
				{
					
					sf::Vector2f direction = pos - particles[spatialHashing.grid[i][j][p]].position;
					float distance = sqrt(pow(direction.x, 2) + pow(direction.y, 2));
					particles[spatialHashing.grid[i][j][p]].Accelerate(direction * std::max(0.f, (150 - distance)) * strength);
					
				}
			}
		}

		
	}

private:
	float absorption = 0.75f;
	float drag = 1.f;

	int boundX;
	int boundY;


	int substeps;
	float subDT;

	void EdgeCheck(int const index)
	{
		sf::Vector2f dx = { -particles[index].GetVelocity().x, particles[index].GetVelocity().y};
		sf::Vector2f dy = { particles[index].GetVelocity().x, -particles[index].GetVelocity().y };

		if (particles[index].position.x + particleDiameter > boundX || particles[index].position.x < 0)
		{
			if (particles[index].position.x + particleDiameter > boundX)
			{
				particles[index].position.x = boundX - particleDiameter;
			}
			if (particles[index].position.x < 0)
			{
				particles[index].position.x = 0;
			}
			particles[index].SetVelocity(dx * absorption);
		}
		if (particles[index].position.y + particleDiameter > boundY || particles[index].position.y< 0)
		{
			if (particles[index].position.y + particleDiameter > boundY)
			{
				particles[index].position.y = boundY - particleDiameter;
			}
			if (particles[index].position.y < 0)
			{
				particles[index].position.y = 0;
			}
			particles[index].SetVelocity(dy * absorption);
		}
	}

	sf::Vector2f ObjectCollision(sf::Vector2f const v, sf::Vector2f const n) const
	{
		return (2 * (v.x * n.x + v.y * n.y) * n - v ) * absorption;
	}

	
	inline void ParticleCollision(uint32_t id, uint32_t yID, uint32_t xID)
	{
		if (yID >= spatialHashing.rowsY || xID >= spatialHashing.columsX || yID < 0 || xID < 0) return;
		for (uint32_t i = 0; i < spatialHashing.grid[yID][xID].size(); i++)
		{
			if (id == spatialHashing.grid[yID][xID][i]) continue;
			

			sf::Vector2f v = particles[id].position - particles[spatialHashing.grid[yID][xID][i]].position;
			float distance = (v.x * v.x) + (v.y * v.y);

			if (distance < minCollision)
			{
				distance = sqrt(distance);
				sf::Vector2f change = v / distance * (0.25f * (particleDiameter - distance));
				particles[id].position += change;
				particles[spatialHashing.grid[yID][xID][i]].position -= change;
				//std::cout << "Particles Collided ID: " << o << " and " << i << " Grid From: X: " << sX << "|Y: " << sY << " Grid To: X:" << tX << "|Y: " << tY << " Change: " << change.x << "|" << change.y << "\n";
			}
		}
	}
	

	inline void CheckGrid(uint32_t yID, uint32_t xID)
	{
		for (uint32_t i = 0; i < spatialHashing.grid[yID][xID].size(); i++)
		{
			ParticleCollision(spatialHashing.grid[yID][xID][i], yID, xID);
			ParticleCollision(spatialHashing.grid[yID][xID][i], yID - 1, xID - 1);
			ParticleCollision(spatialHashing.grid[yID][xID][i], yID - 1, xID);
			ParticleCollision(spatialHashing.grid[yID][xID][i], yID - 1, xID + 1);
			ParticleCollision(spatialHashing.grid[yID][xID][i], yID, xID - 1);
			ParticleCollision(spatialHashing.grid[yID][xID][i], yID, xID + 1);
			ParticleCollision(spatialHashing.grid[yID][xID][i], yID + 1, xID - 1);
			ParticleCollision(spatialHashing.grid[yID][xID][i], yID + 1, xID);
			ParticleCollision(spatialHashing.grid[yID][xID][i], yID + 1, xID + 1);
		}
	}

	void FillGrid()
	{
		//spatialHashing.ClearGrid();

		int x;
		int y;

		for (int i = 0; i < particles.size(); i++)
		{
			x = particles[i].position.x / spatialHashing.cellSize;
			y = particles[i].position.y / spatialHashing.cellSize;

			if (y < 0 || y >= spatialHashing.rowsY || x < 0 || x >= spatialHashing.columsX) continue;
			spatialHashing.grid[y][x].emplace_back(i);
		}
	}

	inline void FillRange(uint32_t start, uint32_t span, uint32_t leftOver)
	{
		uint32_t end = start + span + leftOver;

		int x;
		int y;

		for (uint32_t i = start; i < end; i++)
		{
			x = particles[i].position.x / spatialHashing.cellSize;
			y = particles[i].position.y / spatialHashing.cellSize;

			if (x < 0 || x >= spatialHashing.columsX) continue;
			if (y < 0 || y >= spatialHashing.rowsY) continue;
			spatialHashing.grid[y][x].emplace_back(i);
		}

	}

	inline void ClearRange(uint32_t start, uint32_t span, uint32_t leftOver)
	{
		uint32_t end = start + span + leftOver;
		for (uint32_t y = start; y < end; y++) for (uint32_t x = 0; x < spatialHashing.columsX; x++) spatialHashing.grid[y][x].clear();
	}

	inline void UpdateRange(uint32_t start, uint32_t span, uint32_t leftOver)
	{
		uint32_t end = start + span + leftOver;

		for (uint32_t i = start; i < end; i++)
		{
			if (gravityON)
			{
				//particles[i].acceleration += (sf::Vector2f(boundX * 0.5, boundY * 0.5) - particles[i].position).normalized() * gravityMultiplier * 9.81f / DT;
				particles[i].acceleration += gravity * gravityMultiplier / DT;

			}

			EdgeCheck(i);

			particles[i].Update(subDT);

		}
	}

	inline void DoSlice(uint32_t start, uint32_t span, uint32_t leftOver)
	{
		uint32_t end = start + span + leftOver;
		uint32_t midPoint = start + (span / 2);
		
		for (int y = start; y < midPoint; y++)
		{
			for (int x = 0; x < spatialHashing.columsX; x++)
			{
				if (!spatialHashing.grid[y][x].empty()) CheckGrid(y, x);
			}
		}

		for (int y = midPoint; y < end; y++)
		{
			for (int x = 0; x < spatialHashing.columsX; x++)
			{
				if (!spatialHashing.grid[y][x].empty()) CheckGrid(y, x);
			}
		}
	}
};