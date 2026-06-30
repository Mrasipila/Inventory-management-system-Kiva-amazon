#pragma once

#include "GridPos.h"
#include "WarehouseMap.h"

#include <unordered_set>
#include <vector>

class PathFinder {
public:
    std::vector<GridPos> FindPath(
        const WarehouseMap& map,
        GridPos start,
        GridPos goal,
        const std::unordered_set<GridPos, GridPosHasher>& reserved = {}) const;
};
