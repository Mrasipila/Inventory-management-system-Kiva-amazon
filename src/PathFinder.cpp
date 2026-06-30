#include "PathFinder.h"

#include <algorithm>
#include <limits>
#include <queue>
#include <unordered_map>

namespace {
std::vector<GridPos> ReconstructPath(
    const std::unordered_map<GridPos, GridPos, GridPosHasher>& cameFrom,
    GridPos current) {
    std::vector<GridPos> path;
    path.push_back(current);
    auto it = cameFrom.find(current);
    while (it != cameFrom.end()) {
        current = it->second;
        path.push_back(current);
        it = cameFrom.find(current);
    }
    std::reverse(path.begin(), path.end());
    return path;
}
}

std::vector<GridPos> PathFinder::FindPath(
    const WarehouseMap& map,
    GridPos start,
    GridPos goal,
    const std::unordered_set<GridPos, GridPosHasher>& reserved) const {
    struct Node {
        GridPos cell;
        float f{0.0f};
        float g{0.0f};
    };

    struct Compare {
        bool operator()(const Node& lhs, const Node& rhs) const {
            return lhs.f > rhs.f;
        }
    };

    const GridPos directions[4] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    auto heuristic = [](const GridPos& lhs, const GridPos& rhs) {
        return static_cast<float>(ManhattanDistance(lhs, rhs));
    };

    std::priority_queue<Node, std::vector<Node>, Compare> open;
    std::unordered_map<GridPos, float, GridPosHasher> gScore;
    std::unordered_map<GridPos, GridPos, GridPosHasher> cameFrom;

    open.push({start, heuristic(start, goal), 0.0f});
    gScore[start] = 0.0f;

    while (!open.empty()) {
        Node current = open.top();
        open.pop();

        if (current.cell == goal) {
            return ReconstructPath(cameFrom, current.cell);
        }

        for (const GridPos& direction : directions) {
            GridPos next = current.cell + direction;
            if (!map.InBounds(next)) {
                continue;
            }
            if (!map.Walkable(next) && !(next == goal)) {
                continue;
            }
            if (reserved.count(next) > 0 && !(next == goal)) {
                continue;
            }

            float tentativeG = current.g + 1.0f;
            auto it = gScore.find(next);
            if (it == gScore.end() || tentativeG < it->second) {
                cameFrom[next] = current.cell;
                gScore[next] = tentativeG;
                open.push({next, tentativeG + heuristic(next, goal), tentativeG});
            }
        }
    }

    return {};
}
