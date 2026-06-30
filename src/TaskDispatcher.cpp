#include "TaskDispatcher.h"

#include <limits>

namespace {
const SKU* FindSku(const std::vector<SKU>& skus, int skuId) {
    for (const SKU& sku : skus) {
        if (sku.id == skuId) {
            return &sku;
        }
    }
    return nullptr;
}

GridPos BestStationForShelf(const WarehouseMap& map, const GridPos& shelfCell) {
    const std::vector<GridPos>& stations = map.GetStationCells();
    if (stations.empty()) {
        return map.GetStationCell();
    }

    GridPos bestStation = stations.front();
    int bestDistance = ManhattanDistance(bestStation, shelfCell);
    for (const GridPos& station : stations) {
        int distance = ManhattanDistance(station, shelfCell);
        if (distance < bestDistance) {
            bestDistance = distance;
            bestStation = station;
        }
    }
    return bestStation;
}
}

std::vector<Task> TaskDispatcher::CreateTasksForOrder(
    const Order& order,
    std::vector<Pod>& pods,
    const WarehouseMap& map,
    const std::vector<SKU>& skus,
    int& nextTaskId,
    int& stockouts) const {
    std::vector<Task> tasks;

    for (const OrderLine& line : order.GetLines()) {
        const SKU* sku = FindSku(skus, line.skuId);
        if (sku == nullptr) {
            ++stockouts;
            continue;
        }

        float bestScore = std::numeric_limits<float>::max();
        Pod* bestPod = nullptr;
        GridPos bestStation = map.GetStationCell();

        for (Pod& pod : pods) {
            if (pod.IsReserved() || pod.IsCarried()) {
                continue;
            }
            if (!pod.HasSku(line.skuId, line.quantity)) {
                continue;
            }

            GridPos station = BestStationForShelf(map, pod.GetCurrentCell());
            float distanceScore = static_cast<float>(ManhattanDistance(station, pod.GetCurrentCell()));
            float quantityScore = static_cast<float>(100 - pod.GetAvailableQuantity(line.skuId));
            float velocityBias = static_cast<float>(sku->velocityClass) * 2.0f;
            float score = (0.7f * distanceScore) + (0.2f * quantityScore) + (0.1f * velocityBias);
            if (score < bestScore) {
                bestScore = score;
                bestPod = &pod;
                bestStation = station;
            }
        }

        if (bestPod == nullptr) {
            ++stockouts;
            continue;
        }

        if (!bestPod->ReserveSku(line.skuId, line.quantity)) {
            ++stockouts;
            continue;
        }
        bestPod->SetReserved(true);

        tasks.emplace_back(
            nextTaskId++,
            order.GetId(),
            line.skuId,
            line.quantity,
            bestPod->GetId(),
            bestPod->GetCurrentCell(),
            bestStation,
            order.GetCreatedAt());
    }

    return tasks;
}
