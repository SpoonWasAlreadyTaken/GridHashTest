#pragma once
#include "Particle.hpp"
#include "PhysicsSolver.hpp"
#include "FaultyUtilities.hpp"





struct Cell 
{
public:
	std::vector<int> members;

	Cell()
	{
		members.reserve(overhead);
		size = 0;

		for (int i = 0; i < overhead; i++)
		{
			members.emplace_back(-1);
		}
	}

	bool empty()
	{
		if (size == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void clear()
	{
		size = 0;
	}

	void AddMember(int index)
	{
		members[size] = index;
		size++;
	}

	int Size()
	{
		return size;
	}


private:
	int size;
	int overhead = 5;
};


class SpatialHashing
{
public:
	std::vector<Cell> grid;

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