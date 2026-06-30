#pragma once

#include "GridPos.h"

#include <raylib.h>

#include <vector>

class WarehouseMap {
public:
    WarehouseMap() = default;

    void InitializeDefault();
    bool InBounds(const GridPos& cell) const;
    bool Walkable(const GridPos& cell) const;
    const GridPos& GetStationCell() const;
    const std::vector<GridPos>& GetStationCells() const;
    const std::vector<GridPos>& GetPodCells() const;
    const std::vector<std::vector<bool>>& GetBlockedCells() const;
    Vector3 CellToWorld(const GridPos& cell, float yOffset = 0.0f) const;
    int GetWidth() const;
    int GetHeight() const;
    float GetCellSize() const;

private:
    int width_{40};
    int height_{24};
    float cellSize_{1.2f};
    GridPos stationCell_{2, 12};
    std::vector<GridPos> stationCells_;
    std::vector<std::vector<bool>> blockedCells_;
    std::vector<GridPos> podCells_;
};
