#pragma once
#include "Particle.hpp"
#include "PhysicsSolver.hpp"
#include "FaultyUtilities.hpp"


class SpatialHashing
{
public:
	std::vector<std::vector<int>> grid;

	int cellSize;

	int columsX;
	int rowsY;

	int gridCount;

		
	void ClearGrid()
	{
		for (int i = 0; i < gridCount; i++)
		{
			if (!grid[i].empty())
			{
				grid[i].clear();
			}
		}
	}


private:



};