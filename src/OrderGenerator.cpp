#include "OrderGenerator.h"

#include <algorithm>

OrderGenerator::OrderGenerator(unsigned int seed)
    : rng_{seed} {}

Order OrderGenerator::Generate(int orderId, double now, const std::vector<SKU>& skus) {
    std::uniform_int_distribution<int> lineCountDist(1, 3);
    std::uniform_real_distribution<float> weightDist(0.0f, 1.0f);
    std::uniform_int_distribution<int> quantityDist(1, 4);

    Order order(orderId, now);
    int lineCount = lineCountDist(rng_);

    std::vector<float> cumulative;
    cumulative.reserve(skus.size());
    float sum = 0.0f;
    for (const SKU& sku : skus) {
        sum += sku.demandWeight;
        cumulative.push_back(sum);
    }

    for (int i = 0; i < lineCount; ++i) {
        float sample = weightDist(rng_) * sum;
        auto it = std::lower_bound(cumulative.begin(), cumulative.end(), sample);
        int index = static_cast<int>(std::distance(cumulative.begin(), it));
        index = std::clamp(index, 0, static_cast<int>(skus.size()) - 1);
        order.AddLine({skus[static_cast<std::size_t>(index)].id, quantityDist(rng_)});
    }

    return order;
}
