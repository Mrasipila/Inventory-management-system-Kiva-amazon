#pragma once

#include "GridPos.h"

class Task {
public:
    enum class State {
        Queued,
        Assigned,
        Done
    };

    Task() = default;
    Task(int id, int orderId, int skuId, int quantity, int podId, GridPos podCell, GridPos stationCell, double createdAt);

    int GetId() const;
    int GetOrderId() const;
    int GetSkuId() const;
    int GetQuantity() const;
    int GetPodId() const;
    const GridPos& GetPodCell() const;
    const GridPos& GetStationCell() const;
    State GetState() const;
    void SetState(State state);
    int GetAssignedRobotId() const;
    void SetAssignedRobotId(int robotId);
    double GetCreatedAt() const;
    double GetCompletedAt() const;
    void SetCompletedAt(double value);

private:
    int id_{0};
    int orderId_{0};
    int skuId_{0};
    int quantity_{0};
    int podId_{0};
    GridPos podCell_{};
    GridPos stationCell_{};
    State state_{State::Queued};
    int assignedRobotId_{-1};
    double createdAt_{0.0};
    double completedAt_{0.0};
};
