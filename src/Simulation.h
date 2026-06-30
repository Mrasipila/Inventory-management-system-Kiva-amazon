#pragma once

#include "OrderGenerator.h"
#include "PathFinder.h"
#include "Pod.h"
#include "Robot.h"
#include "SKU.h"
#include "Task.h"
#include "TaskDispatcher.h"
#include "WarehouseMap.h"

#include <random>
#include <vector>

struct Metrics {
    int ordersCompleted{0};
    int tasksCompleted{0};
    int stockouts{0};
    float totalTravelDistance{0.0f};
    double totalOrderCycle{0.0};
};

class Simulation {
public:
    Simulation();

    void Reset();
    void Update(float dt);

    const WarehouseMap& GetMap() const;
    const std::vector<SKU>& GetSkus() const;
    const std::vector<Pod>& GetPods() const;
    const std::vector<Order>& GetOrders() const;
    const std::vector<Task>& GetTasks() const;
    const std::vector<Robot>& GetRobots() const;
    const Metrics& GetMetrics() const;
    double GetTime() const;

private:
    void SeedSkus();
    void SeedPods();
    void SeedRobots();
    void ReplenishInventoryIfNeeded();
    void SpawnOrderIfNeeded();
    void DispatchQueuedTasks();
    void UpdateRobots(float dt);
    void FinalizeOrders();
    Task* FindTaskById(int taskId);
    Pod* FindPodById(int podId);
    Order* FindOrderById(int orderId);

    WarehouseMap map_;
    PathFinder pathFinder_;
    OrderGenerator orderGenerator_;
    TaskDispatcher taskDispatcher_;
    std::vector<SKU> skus_;
    std::vector<Pod> pods_;
    std::vector<Order> orders_;
    std::vector<Task> tasks_;
    std::vector<Robot> robots_;
    Metrics metrics_;
    int nextOrderId_{1};
    int nextTaskId_{1};
    double now_{0.0};
    double nextOrderSpawn_{1.5};
    double nextReplenishment_{12.0};
    std::mt19937 rng_;
};
