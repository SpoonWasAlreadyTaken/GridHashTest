#pragma once


class SpatialHashing
{
public:
	std::vector<std::vector<int>> grid;

	uint32_t cellSize;

	uint32_t gridX;
	uint32_t gridY;

	uint32_t gridCount;


	void ClearGrid() { for (uint32_t i = 0; i < grid.size(); i++) grid[i].clear(); }
};
