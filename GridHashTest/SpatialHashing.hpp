#pragma once
#include "Particle.hpp"
#include "PhysicsSolver.hpp"
#include "FaultyUtilities.hpp"


class SpatialHashing
{
public:
	std::vector<std::vector<int>> grid;

	int cellSize;

	int gridX;
	int gridY;

	int gridCount;


	void ClearGrid()
	{
		for (int i = 0; i < grid.size(); i++) grid[i].clear();
	}
};