#include "GridPos.h"

#include <cmath>
#include <functional>

bool operator==(const GridPos& lhs, const GridPos& rhs) noexcept {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

GridPos operator+(const GridPos& lhs, const GridPos& rhs) noexcept {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

int ManhattanDistance(const GridPos& lhs, const GridPos& rhs) noexcept {
    return std::abs(lhs.x - rhs.x) + std::abs(lhs.y - rhs.y);
}

std::size_t GridPosHasher::operator()(const GridPos& value) const noexcept {
    std::size_t h1 = std::hash<int>{}(value.x);
    std::size_t h2 = std::hash<int>{}(value.y);
    return h1 ^ (h2 << 1);
}
