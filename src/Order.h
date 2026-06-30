#pragma once

#include "OrderLine.h"

#include <vector>

class Order {
public:
    Order() = default;
    Order(int id, double createdAt);

    int GetId() const;
    double GetCreatedAt() const;
    bool IsCompleted() const;
    void SetCompleted(bool value);
    const std::vector<OrderLine>& GetLines() const;
    std::vector<OrderLine>& GetLines();
    void AddLine(const OrderLine& line);

private:
    int id_{0};
    double createdAt_{0.0};
    bool completed_{false};
    std::vector<OrderLine> lines_;
};
