#include "Simulation.h"

#include <algorithm>
#include <deque>
#include <limits>
#include <string>
#include <unordered_set>

namespace {
constexpr float ReplanAfterBlockedSeconds = 0.55f;
constexpr float PickStationServiceSeconds = 0.85f;

std::vector<GridPos> BuildPath(
    const PathFinder& pathFinder,
    const WarehouseMap& map,
    const GridPos& start,
    const GridPos& goal,
    const std::unordered_set<GridPos, GridPosHasher>& reserved) {
    return pathFinder.FindPath(map, start, goal, reserved);
}

GridPos GoalForRobotState(const Robot& robot, const Task* task, const Pod* pod, const WarehouseMap& map) {
    if (robot.GetState() == Robot::State::ToPod && task != nullptr) {
        return task->GetPodCell();
    }
    if (robot.GetState() == Robot::State::ToStation && task != nullptr) {
        return task->GetStationCell();
    }
    if (robot.GetState() == Robot::State::PickingAtStation && task != nullptr) {
        return task->GetStationCell();
    }
    if (robot.GetState() == Robot::State::ReturningHome && pod != nullptr) {
        return pod->GetHomeCell();
    }
    return robot.GetCell();
}

std::unordered_set<GridPos, GridPosHasher> BuildReservedCells(const std::vector<Robot>& robots, int movingRobotId) {
    std::unordered_set<GridPos, GridPosHasher> reserved;
    for (const Robot& other : robots) {
        if (other.GetId() != movingRobotId) {
            reserved.insert(other.GetCell());
        }
    }
    return reserved;
}

void ReplanRobotPath(
    Robot& robot,
    const Task* task,
    const Pod* pod,
    const WarehouseMap& map,
    const PathFinder& pathFinder,
    const std::vector<Robot>& robots) {
    GridPos goal = GoalForRobotState(robot, task, pod, map);
    if (goal == robot.GetCell()) {
        return;
    }

    std::unordered_set<GridPos, GridPosHasher> reserved = BuildReservedCells(robots, robot.GetId());
    std::vector<GridPos> path = pathFinder.FindPath(map, robot.GetCell(), goal, reserved);

    if (path.empty()) {
        path = pathFinder.FindPath(map, robot.GetCell(), goal);
    }
    if (!path.empty()) {
        path.erase(path.begin());
        robot.SetPath(std::deque<GridPos>(path.begin(), path.end()));
        robot.ResetBlockedTime();
    }
}
}

Simulation::Simulation()
    : orderGenerator_{42}, rng_{42} {
    Reset();
}

void Simulation::Reset() {
    map_ = WarehouseMap{};
    map_.InitializeDefault();

    skus_.clear();
    pods_.clear();
    orders_.clear();
    tasks_.clear();
    robots_.clear();
    metrics_ = {};
    nextOrderId_ = 1;
    nextTaskId_ = 1;
    now_ = 0.0;
    nextOrderSpawn_ = 0.4;
    nextReplenishment_ = 12.0;
    orderGenerator_ = OrderGenerator{42};
    rng_.seed(42);

    SeedSkus();
    SeedPods();
    SeedRobots();
}

void Simulation::SeedSkus() {
    for (int i = 0; i < 60; ++i) {
        SKU sku;
        sku.id = i + 1;
        sku.name = "SKU_" + std::to_string(i + 1);
        sku.weight = 0.5f + static_cast<float>(i % 7) * 0.2f;
        sku.volume = 0.3f + static_cast<float>(i % 5) * 0.1f;

        if (i < 12) {
            sku.velocityClass = 0;
            sku.demandWeight = 8.0f;
        } else if (i < 30) {
            sku.velocityClass = 1;
            sku.demandWeight = 3.0f;
        } else {
            sku.velocityClass = 2;
            sku.demandWeight = 1.0f;
        }

        skus_.push_back(sku);
    }
}

void Simulation::SeedPods() {
    const std::vector<GridPos>& podCells = map_.GetPodCells();
    std::uniform_int_distribution<int> qtyDist(30, 75);

    int podId = 1;
    for (int cellIndex = 0; cellIndex < static_cast<int>(podCells.size()); ++cellIndex) {
        const GridPos& cell = podCells[static_cast<std::size_t>(cellIndex)];
        Pod pod(podId++, cell);

        for (int offset = 0; offset < 5; ++offset) {
            int skuIndex = (cellIndex * 5 + offset) % static_cast<int>(skus_.size());
            pod.AddSku(skus_[static_cast<std::size_t>(skuIndex)].id, qtyDist(rng_));
        }

        pods_.push_back(pod);
    }
}

void Simulation::ReplenishInventoryIfNeeded() {
    if (now_ < nextReplenishment_ || pods_.empty() || skus_.empty()) {
        return;
    }

    std::uniform_int_distribution<int> podDist(0, static_cast<int>(pods_.size()) - 1);
    std::uniform_int_distribution<int> qtyDist(20, 45);

    for (const SKU& sku : skus_) {
        for (int copy = 0; copy < 2; ++copy) {
            Pod& pod = pods_[static_cast<std::size_t>(podDist(rng_))];
            pod.AddSku(sku.id, qtyDist(rng_));
        }
    }

    nextReplenishment_ = now_ + 12.0;
}

void Simulation::SeedRobots() {
    const std::vector<GridPos>& stations = map_.GetStationCells();
    constexpr int RobotCount = 14;
    const GridPos offsets[] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}, {2, 0}, {-2, 0}
    };

    std::unordered_set<GridPos, GridPosHasher> usedStarts;

    for (int i = 0; i < RobotCount; ++i) {
        GridPos station = stations.empty()
            ? map_.GetStationCell()
            : stations[static_cast<std::size_t>(i % static_cast<int>(stations.size()))];

        GridPos start = station;
        for (const GridPos& offset : offsets) {
            GridPos candidate = station + offset;
            if (map_.Walkable(candidate) && usedStarts.count(candidate) == 0) {
                start = candidate;
                break;
            }
        }
        usedStarts.insert(start);

        Robot robot(i, start);
        robot.SetHomeCell(start);
        robot.SetSpeedCellsPerSecond(3.5f);
        robots_.push_back(robot);
    }
}

void Simulation::SpawnOrderIfNeeded() {
    if (now_ < nextOrderSpawn_) {
        return;
    }

    Order order = orderGenerator_.Generate(nextOrderId_++, now_, skus_);
    orders_.push_back(order);

    std::vector<Task> generated = taskDispatcher_.CreateTasksForOrder(
        orders_.back(),
        pods_,
        map_,
        skus_,
        nextTaskId_,
        metrics_.stockouts);

    for (Task& task : generated) {
        tasks_.push_back(task);
    }

    std::uniform_real_distribution<double> nextSpawnDist(0.45, 1.15);
    nextOrderSpawn_ = now_ + nextSpawnDist(rng_);
}

void Simulation::DispatchQueuedTasks() {
    for (Task& task : tasks_) {
        if (task.GetState() != Task::State::Queued) {
            continue;
        }

        float bestDistance = std::numeric_limits<float>::max();
        Robot* bestRobot = nullptr;
        for (Robot& robot : robots_) {
            if (!robot.IsIdle()) {
                continue;
            }
            float distance = static_cast<float>(ManhattanDistance(robot.GetCell(), task.GetPodCell()));
            if (distance < bestDistance) {
                bestDistance = distance;
                bestRobot = &robot;
            }
        }

        if (bestRobot == nullptr) {
            continue;
        }

        std::unordered_set<GridPos, GridPosHasher> reserved;
        for (const Robot& other : robots_) {
            if (other.GetId() != bestRobot->GetId()) {
                reserved.insert(other.GetCell());
            }
        }

        std::vector<GridPos> path = BuildPath(pathFinder_, map_, bestRobot->GetCell(), task.GetPodCell(), reserved);
        if (path.empty()) {
            continue;
        }

        if (!path.empty()) {
            path.erase(path.begin());
        }

        bestRobot->SetPath(std::deque<GridPos>(path.begin(), path.end()));
        bestRobot->SetState(Robot::State::ToPod);
        bestRobot->SetActiveTaskId(task.GetId());
        task.SetAssignedRobotId(bestRobot->GetId());
        task.SetState(Task::State::Assigned);
    }
}

Task* Simulation::FindTaskById(int taskId) {
    for (Task& task : tasks_) {
        if (task.GetId() == taskId) {
            return &task;
        }
    }
    return nullptr;
}

Pod* Simulation::FindPodById(int podId) {
    for (Pod& pod : pods_) {
        if (pod.GetId() == podId) {
            return &pod;
        }
    }
    return nullptr;
}

Order* Simulation::FindOrderById(int orderId) {
    for (Order& order : orders_) {
        if (order.GetId() == orderId) {
            return &order;
        }
    }
    return nullptr;
}

void Simulation::UpdateRobots(float dt) {
    std::unordered_set<GridPos, GridPosHasher> occupied;
    for (const Robot& robot : robots_) {
        occupied.insert(robot.GetCell());
    }

    for (Robot& robot : robots_) {
        if (robot.IsIdle()) {
            continue;
        }

        Task* task = FindTaskById(robot.GetActiveTaskId());
        Pod* pod = nullptr;
        if (task != nullptr) {
            pod = FindPodById(task->GetPodId());
        } else if (robot.GetCarriedPodId() >= 0) {
            pod = FindPodById(robot.GetCarriedPodId());
        }

        if (robot.GetState() == Robot::State::ReturningHome) {
            if (pod != nullptr && robot.GetCell() == pod->GetHomeCell()) {
                pod->SetCarried(false);
                pod->SetCarryingRobotId(-1);
                pod->SetReserved(false);
                pod->SetCurrentCell(pod->GetHomeCell());
                robot.SetCarriedPodId(-1);
                robot.SetState(Robot::State::Idle);
                robot.SetActiveTaskId(-1);
                robot.ClearPath();
                robot.ResetMoveAccumulator();
                continue;
            }

            if (robot.GetPath().empty() && pod != nullptr) {
                std::unordered_set<GridPos, GridPosHasher> reserved;
                for (const Robot& other : robots_) {
                    if (other.GetId() != robot.GetId()) {
                        reserved.insert(other.GetCell());
                    }
                }

                std::vector<GridPos> homePath = pathFinder_.FindPath(map_, robot.GetCell(), pod->GetHomeCell(), reserved);
                if (!homePath.empty()) {
                    homePath.erase(homePath.begin());
                    robot.SetPath(std::deque<GridPos>(homePath.begin(), homePath.end()));
                }
            }
        }

        if (robot.GetState() == Robot::State::PickingAtStation) {
            if (task == nullptr || pod == nullptr) {
                robot.SetState(Robot::State::Idle);
                robot.SetActiveTaskId(-1);
                robot.SetCarriedPodId(-1);
                robot.ResetServiceTime();
                continue;
            }

            robot.AddServiceTime(dt);
            pod->SetCurrentCell(robot.GetCell());

            if (robot.GetServiceTime() < PickStationServiceSeconds) {
                continue;
            }

            task->SetState(Task::State::Done);
            task->SetCompletedAt(now_);
            metrics_.tasksCompleted += 1;
            metrics_.totalTravelDistance += robot.GetDistanceTravelled();
            robot.ResetDistanceTravelled();
            robot.ResetServiceTime();

            std::unordered_set<GridPos, GridPosHasher> reserved;
            for (const Robot& other : robots_) {
                if (other.GetId() != robot.GetId()) {
                    reserved.insert(other.GetCell());
                }
            }

            std::vector<GridPos> homePath = pathFinder_.FindPath(map_, robot.GetCell(), pod->GetHomeCell(), reserved);
            if (homePath.empty()) {
                homePath = pathFinder_.FindPath(map_, robot.GetCell(), pod->GetHomeCell());
            }
            if (!homePath.empty()) {
                homePath.erase(homePath.begin());
            }
            robot.SetPath(std::deque<GridPos>(homePath.begin(), homePath.end()));
            robot.SetState(Robot::State::ReturningHome);
        }

        if (robot.GetPath().empty()) {
            continue;
        }

        robot.AddMoveAccumulator(robot.GetSpeedCellsPerSecond() * dt);
        while (robot.GetMoveAccumulator() >= 1.0f && !robot.GetPath().empty()) {
            GridPos next = robot.GetPath().front();
            if (occupied.count(next) > 0) {
                robot.AddBlockedTime(dt);
                if (robot.GetBlockedTime() >= ReplanAfterBlockedSeconds) {
                    ReplanRobotPath(robot, task, pod, map_, pathFinder_, robots_);
                    robot.ResetMoveAccumulator();
                }
                break;
            }

            occupied.erase(robot.GetCell());
            robot.SetCell(next);
            occupied.insert(robot.GetCell());
            robot.GetPath().pop_front();
            robot.AddMoveAccumulator(-1.0f);
            robot.AddDistanceTravelled(1.0f);
            robot.ResetBlockedTime();

            if (pod != nullptr && pod->IsCarried()) {
                pod->SetCurrentCell(robot.GetCell());
            }

            if (task == nullptr) {
                robot.SetState(Robot::State::Idle);
                robot.SetActiveTaskId(-1);
                robot.ClearPath();
                break;
            }

            if (pod == nullptr) {
                robot.SetState(Robot::State::Idle);
                robot.SetActiveTaskId(-1);
                robot.ClearPath();
                break;
            }

            if (robot.GetState() == Robot::State::ToPod && robot.GetCell() == task->GetPodCell()) {
                pod->SetCarried(true);
                pod->SetCarryingRobotId(robot.GetId());
                pod->SetCurrentCell(robot.GetCell());
                robot.SetCarriedPodId(pod->GetId());

                std::unordered_set<GridPos, GridPosHasher> reserved;
                for (const Robot& other : robots_) {
                    if (other.GetId() != robot.GetId()) {
                        reserved.insert(other.GetCell());
                    }
                }

                std::vector<GridPos> toStation = pathFinder_.FindPath(map_, robot.GetCell(), task->GetStationCell(), reserved);
                if (!toStation.empty()) {
                    toStation.erase(toStation.begin());
                }
                robot.SetPath(std::deque<GridPos>(toStation.begin(), toStation.end()));
                robot.SetState(Robot::State::ToStation);
            }

            if (robot.GetState() == Robot::State::ToStation && robot.GetCell() == task->GetStationCell() && robot.GetPath().empty()) {
                robot.ClearPath();
                robot.ResetMoveAccumulator();
                robot.ResetServiceTime();
                robot.SetState(Robot::State::PickingAtStation);
            }

            if (robot.GetState() == Robot::State::ReturningHome && pod != nullptr) {
                pod->SetCurrentCell(robot.GetCell());
                if (robot.GetCell() == pod->GetHomeCell() && robot.GetPath().empty()) {
                    pod->SetCarried(false);
                    pod->SetCarryingRobotId(-1);
                    pod->SetReserved(false);
                    robot.SetCarriedPodId(-1);
                    robot.SetState(Robot::State::Idle);
                    robot.SetActiveTaskId(-1);
                    robot.ResetMoveAccumulator();
                    break;
                }
            }
        }
    }

    for (Pod& pod : pods_) {
        if (!pod.IsCarried()) {
            pod.SetCurrentCell(pod.GetHomeCell());
        }
    }
}

void Simulation::FinalizeOrders() {
    for (Order& order : orders_) {
        if (order.IsCompleted()) {
            continue;
        }

        bool allDone = true;
        for (const Task& task : tasks_) {
            if (task.GetOrderId() == order.GetId() && task.GetState() != Task::State::Done) {
                allDone = false;
                break;
            }
        }

        if (allDone) {
            order.SetCompleted(true);
            metrics_.ordersCompleted += 1;
            metrics_.totalOrderCycle += (now_ - order.GetCreatedAt());
        }
    }
}

void Simulation::Update(float dt) {
    now_ += dt;
    ReplenishInventoryIfNeeded();
    SpawnOrderIfNeeded();
    DispatchQueuedTasks();
    UpdateRobots(dt);
    FinalizeOrders();
}

const WarehouseMap& Simulation::GetMap() const {
    return map_;
}

const std::vector<SKU>& Simulation::GetSkus() const {
    return skus_;
}

const std::vector<Pod>& Simulation::GetPods() const {
    return pods_;
}

const std::vector<Order>& Simulation::GetOrders() const {
    return orders_;
}

const std::vector<Task>& Simulation::GetTasks() const {
    return tasks_;
}

const std::vector<Robot>& Simulation::GetRobots() const {
    return robots_;
}

const Metrics& Simulation::GetMetrics() const {
    return metrics_;
}

double Simulation::GetTime() const {
    return now_;
}
