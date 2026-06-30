#include "WarehouseMap.h"

void WarehouseMap::InitializeDefault() {
    blockedCells_.assign(static_cast<std::size_t>(height_), std::vector<bool>(static_cast<std::size_t>(width_), false));
    podCells_.clear();
    stationCells_ = {
        {2, 5},
        {2, 12},
        {2, 19},
        {width_ - 3, 5},
        {width_ - 3, 12},
        {width_ - 3, 19}
    };
    stationCell_ = stationCells_.front();

    for (int x = 8; x < width_ - 2; x += 5) {
        for (int y = 2; y < height_ - 2; ++y) {
            if ((y % 6) == 0) {
                continue;
            }

            blockedCells_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = true;
            podCells_.push_back({x, y});
        }
    }
}

bool WarehouseMap::InBounds(const GridPos& cell) const {
    return cell.x >= 0 && cell.x < width_ && cell.y >= 0 && cell.y < height_;
}

bool WarehouseMap::Walkable(const GridPos& cell) const {
    return InBounds(cell) && !blockedCells_[static_cast<std::size_t>(cell.y)][static_cast<std::size_t>(cell.x)];
}

const GridPos& WarehouseMap::GetStationCell() const {
    return stationCell_;
}

const std::vector<GridPos>& WarehouseMap::GetStationCells() const {
    return stationCells_;
}

const std::vector<GridPos>& WarehouseMap::GetPodCells() const {
    return podCells_;
}

const std::vector<std::vector<bool>>& WarehouseMap::GetBlockedCells() const {
    return blockedCells_;
}

Vector3 WarehouseMap::CellToWorld(const GridPos& cell, float yOffset) const {
    float ox = static_cast<float>(width_) * cellSize_ * 0.5f;
    float oz = static_cast<float>(height_) * cellSize_ * 0.5f;
    return {
        cell.x * cellSize_ - ox + (cellSize_ * 0.5f),
        yOffset,
        cell.y * cellSize_ - oz + (cellSize_ * 0.5f)
    };
}

int WarehouseMap::GetWidth() const {
    return width_;
}

int WarehouseMap::GetHeight() const {
    return height_;
}

float WarehouseMap::GetCellSize() const {
    return cellSize_;
}
