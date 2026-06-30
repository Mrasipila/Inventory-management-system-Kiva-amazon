#pragma once

#include <cstddef>

struct GridPos {
    int x{0};
    int y{0};
};

bool operator==(const GridPos& lhs, const GridPos& rhs) noexcept;
GridPos operator+(const GridPos& lhs, const GridPos& rhs) noexcept;
int ManhattanDistance(const GridPos& lhs, const GridPos& rhs) noexcept;

struct GridPosHasher {
    std::size_t operator()(const GridPos& value) const noexcept;
};
