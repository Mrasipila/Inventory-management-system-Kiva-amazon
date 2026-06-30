#pragma once

#include "Order.h"
#include "SKU.h"

#include <random>
#include <vector>

class OrderGenerator {
public:
    explicit OrderGenerator(unsigned int seed = 42);

    Order Generate(int orderId, double now, const std::vector<SKU>& skus);

private:
    std::mt19937 rng_;
};
