#pragma once
#include "Particle.hpp"
#include "PhysicsSolver.hpp"
#include "FaultyUtilities.hpp"


class SpatialHashing
{
public:
	std::vector<int> **grid = NULL;

	int cellSize;

	int columsX;
	int rowsY;

	int gridCount;

		
	void ClearGrid()
	{
		for (int i = 0; i < rowsY; i++)
		{
			for (int j = 0; j < columsX; j++)
			{
				if (!grid[i][j].empty())
				{
					grid[i][j].clear();
				}
			}
		}
	}


private:



};