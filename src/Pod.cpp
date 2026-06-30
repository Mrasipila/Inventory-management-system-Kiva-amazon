#include "Pod.h"

#include <algorithm>

Pod::Pod(int id, GridPos homeCell)
    : id_{id}, homeCell_{homeCell}, currentCell_{homeCell} {}

int Pod::GetId() const {
    return id_;
}

const GridPos& Pod::GetHomeCell() const {
    return homeCell_;
}

const GridPos& Pod::GetCurrentCell() const {
    return currentCell_;
}

void Pod::SetCurrentCell(const GridPos& cell) {
    currentCell_ = cell;
}

bool Pod::IsCarried() const {
    return carried_;
}

void Pod::SetCarried(bool value) {
    carried_ = value;
}

bool Pod::IsReserved() const {
    return reserved_;
}

void Pod::SetReserved(bool value) {
    reserved_ = value;
}

int Pod::GetCarryingRobotId() const {
    return carryingRobotId_;
}

void Pod::SetCarryingRobotId(int robotId) {
    carryingRobotId_ = robotId;
}

bool Pod::HasSku(int skuId, int quantity) const {
    return GetAvailableQuantity(skuId) >= quantity;
}

int Pod::GetAvailableQuantity(int skuId) const {
    auto it = inventory_.find(skuId);
    if (it == inventory_.end()) {
        return 0;
    }
    return it->second;
}

bool Pod::ReserveSku(int skuId, int quantity) {
    auto it = inventory_.find(skuId);
    if (it == inventory_.end() || it->second < quantity) {
        return false;
    }
    it->second -= quantity;
    return true;
}

void Pod::AddSku(int skuId, int quantity) {
    inventory_[skuId] += quantity;
}
