#pragma once

#include "GridPos.h"

#include <unordered_map>

class Pod {
public:
    Pod() = default;
    Pod(int id, GridPos homeCell);

    int GetId() const;
    const GridPos& GetHomeCell() const;
    const GridPos& GetCurrentCell() const;
    void SetCurrentCell(const GridPos& cell);
    bool IsCarried() const;
    void SetCarried(bool value);
    bool IsReserved() const;
    void SetReserved(bool value);
    int GetCarryingRobotId() const;
    void SetCarryingRobotId(int robotId);
    bool HasSku(int skuId, int quantity) const;
    int GetAvailableQuantity(int skuId) const;
    bool ReserveSku(int skuId, int quantity);
    void AddSku(int skuId, int quantity);

private:
    int id_{0};
    GridPos homeCell_{};
    GridPos currentCell_{};
    bool carried_{false};
    bool reserved_{false};
    int carryingRobotId_{-1};
    std::unordered_map<int, int> inventory_;
};
