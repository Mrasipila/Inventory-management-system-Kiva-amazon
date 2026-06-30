#include "Order.h"

Order::Order(int id, double createdAt)
    : id_{id}, createdAt_{createdAt} {}

int Order::GetId() const {
    return id_;
}

double Order::GetCreatedAt() const {
    return createdAt_;
}

bool Order::IsCompleted() const {
    return completed_;
}

void Order::SetCompleted(bool value) {
    completed_ = value;
}

const std::vector<OrderLine>& Order::GetLines() const {
    return lines_;
}

std::vector<OrderLine>& Order::GetLines() {
    return lines_;
}

void Order::AddLine(const OrderLine& line) {
    lines_.push_back(line);
}
