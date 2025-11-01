#pragma once

class SpatialHashing
{
public:
	std::vector<std::vector<int>> grid;

	uint32_t cellSize = GetCellSize();

	uint32_t gridX = GetX();
	uint32_t gridY = GetY();

    size_t gridCount = GetGridCount();


	void ClearGrid() { for (uint32_t i = 0; i < grid.size(); i++) grid[i].clear(); }

    SpatialHashing()
    {
        for (uint32_t i = 0; i < gridCount; i++) grid.emplace_back();

        std::cout << "Grid X: " << gridX << " | Grid Y: " << gridY << "\n";
        std::cout << "Grid Count: " << gridCount << "\n";
    }


    uint32_t GetCellSize() { return ceilf((particleSize) * 2); }
    uint32_t GetX() { return sizeX / ceilf((particleSize) * 2); }
    uint32_t GetY() { return sizeY / ceilf((particleSize) * 2); }
    size_t GetGridCount() { return (sizeX / ceilf((particleSize) * 2)) * (sizeY / ceilf((particleSize) * 2)); }
};
