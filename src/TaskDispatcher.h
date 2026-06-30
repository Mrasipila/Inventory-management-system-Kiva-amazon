#pragma once

#include "Order.h"
#include "Pod.h"
#include "SKU.h"
#include "Task.h"
#include "WarehouseMap.h"

#include <vector>

class TaskDispatcher {
public:
    std::vector<Task> CreateTasksForOrder(
        const Order& order,
        std::vector<Pod>& pods,
        const WarehouseMap& map,
        const std::vector<SKU>& skus,
        int& nextTaskId,
        int& stockouts) const;
};
