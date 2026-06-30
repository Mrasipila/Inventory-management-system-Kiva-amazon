#include "Task.h"

Task::Task(int id, int orderId, int skuId, int quantity, int podId, GridPos podCell, GridPos stationCell, double createdAt)
    : id_{id}, orderId_{orderId}, skuId_{skuId}, quantity_{quantity}, podId_{podId}, podCell_{podCell}, stationCell_{stationCell}, createdAt_{createdAt} {}

int Task::GetId() const {
    return id_;
}

int Task::GetOrderId() const {
    return orderId_;
}

int Task::GetSkuId() const {
    return skuId_;
}

int Task::GetQuantity() const {
    return quantity_;
}

int Task::GetPodId() const {
    return podId_;
}

const GridPos& Task::GetPodCell() const {
    return podCell_;
}

const GridPos& Task::GetStationCell() const {
    return stationCell_;
}

Task::State Task::GetState() const {
    return state_;
}

void Task::SetState(State state) {
    state_ = state;
}

int Task::GetAssignedRobotId() const {
    return assignedRobotId_;
}

void Task::SetAssignedRobotId(int robotId) {
    assignedRobotId_ = robotId;
}

double Task::GetCreatedAt() const {
    return createdAt_;
}

double Task::GetCompletedAt() const {
    return completedAt_;
}

void Task::SetCompletedAt(double value) {
    completedAt_ = value;
}
