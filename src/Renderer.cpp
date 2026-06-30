#include "Renderer.h"

#include <string>

Color Renderer::RobotColor(int id) const {
    static Color palette[] = {
        {220, 53, 69, 255},
        {9, 153, 114, 255},
        {59, 130, 246, 255},
        {245, 158, 11, 255},
        {124, 77, 255, 255},
        {0, 172, 193, 255}
    };
    return palette[id % (sizeof(palette) / sizeof(palette[0]))];
}

void Renderer::DrawWarehouse(const Simulation& simulation) const {
    const WarehouseMap& map = simulation.GetMap();

    Vector3 floorCenter = map.CellToWorld({map.GetWidth() / 2, map.GetHeight() / 2}, -0.05f);
    DrawCube(floorCenter, map.GetWidth() * map.GetCellSize(), 0.1f, map.GetHeight() * map.GetCellSize(), Color{234, 237, 240, 255});
    DrawGrid(map.GetWidth(), map.GetCellSize());

    const auto& blocked = map.GetBlockedCells();
    for (int y = 0; y < map.GetHeight(); ++y) {
        for (int x = 0; x < map.GetWidth(); ++x) {
            if (!blocked[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)]) {
                continue;
            }

            bool isMovableShelfHome = false;
            for (const Pod& pod : simulation.GetPods()) {
                if (pod.GetHomeCell() == GridPos{x, y}) {
                    isMovableShelfHome = true;
                    break;
                }
            }
            if (isMovableShelfHome) {
                continue;
            }

            Vector3 rack = map.CellToWorld({x, y}, 1.05f);
            DrawCube(rack, map.GetCellSize() * 0.85f, 2.1f, map.GetCellSize() * 0.85f, Color{120, 125, 130, 255});
            DrawCubeWires(rack, map.GetCellSize() * 0.85f, 2.1f, map.GetCellSize() * 0.85f, Color{86, 90, 94, 255});
        }
    }

    for (const Pod& pod : simulation.GetPods()) {
        Vector3 shelfWorld = map.CellToWorld(pod.GetCurrentCell(), pod.IsCarried() ? 1.18f : 1.05f);
        DrawCube(shelfWorld, map.GetCellSize() * 0.86f, 2.1f, map.GetCellSize() * 0.86f, Color{118, 123, 128, 255});
        DrawCubeWires(shelfWorld, map.GetCellSize() * 0.86f, 2.1f, map.GetCellSize() * 0.86f, Color{54, 58, 63, 255});
        DrawCube(map.CellToWorld(pod.GetCurrentCell(), pod.IsCarried() ? 0.34f : 0.12f), map.GetCellSize() * 0.76f, 0.12f, map.GetCellSize() * 0.76f, Color{72, 76, 82, 255});
    }

    for (const GridPos& stationCell : map.GetStationCells()) {
        Vector3 station = map.CellToWorld(stationCell, 0.25f);
        DrawCube(station, map.GetCellSize() * 1.2f, 0.5f, map.GetCellSize() * 1.2f, Color{43, 160, 54, 255});
        DrawCubeWires(station, map.GetCellSize() * 1.2f, 0.5f, map.GetCellSize() * 1.2f, Color{20, 96, 35, 255});
    }

    for (const Robot& robot : simulation.GetRobots()) {
        Vector3 robotWorld = map.CellToWorld(robot.GetCell(), 0.23f);
        DrawCube(robotWorld, map.GetCellSize() * 0.5f, 0.42f, map.GetCellSize() * 0.5f, RobotColor(robot.GetId()));

        if (!robot.GetPath().empty()) {
            Vector3 nextPoint = map.CellToWorld(robot.GetPath().front(), 0.18f);
            DrawLine3D(robotWorld, nextPoint, Color{20, 20, 20, 120});
        }
    }
}

void Renderer::DrawHud(const Simulation& simulation) const {
    const Metrics& metrics = simulation.GetMetrics();

    int queuedOrders = 0;
    for (const Order& order : simulation.GetOrders()) {
        if (!order.IsCompleted()) {
            ++queuedOrders;
        }
    }

    int activeTasks = 0;
    for (const Task& task : simulation.GetTasks()) {
        if (task.GetState() != Task::State::Done) {
            ++activeTasks;
        }
    }

    float avgTravel = metrics.tasksCompleted > 0 ? metrics.totalTravelDistance / static_cast<float>(metrics.tasksCompleted) : 0.0f;
    double avgCycle = metrics.ordersCompleted > 0 ? metrics.totalOrderCycle / static_cast<double>(metrics.ordersCompleted) : 0.0;

    DrawRectangle(14, 14, 465, 268, Fade(Color{10, 20, 30, 255}, 0.75f));
    DrawText("Warehouse Goods-to-Person Simulation", 24, 24, 22, RAYWHITE);
    DrawText(TextFormat("Time: %.1fs", simulation.GetTime()), 24, 58, 18, Color{188, 220, 255, 255});
    DrawText(TextFormat("Orders completed: %d", metrics.ordersCompleted), 24, 84, 18, RAYWHITE);
    DrawText(TextFormat("Orders in queue: %d", queuedOrders), 24, 108, 18, RAYWHITE);
    DrawText(TextFormat("Tasks completed: %d", metrics.tasksCompleted), 24, 132, 18, RAYWHITE);
    DrawText(TextFormat("Active tasks: %d", activeTasks), 24, 156, 18, RAYWHITE);
    DrawText(TextFormat("Stockouts: %d", metrics.stockouts), 24, 180, 18, Color{255, 140, 120, 255});
    DrawText(TextFormat("Avg travel/task: %.2f cells", avgTravel), 24, 204, 18, RAYWHITE);
    DrawText(TextFormat("Avg order cycle: %.2fs", avgCycle), 24, 228, 18, RAYWHITE);
    DrawText("R: reset  |  TAB: camera mode", 24, 252, 16, Color{160, 200, 235, 255});
}

void Renderer::Draw(const Simulation& simulation, Camera3D& camera, bool topDown) const {
    if (topDown) {
        camera.position = {0.0f, 60.0f, 0.001f};
        camera.target = {0.0f, 0.0f, 0.0f};
        camera.up = {0.0f, 0.0f, -1.0f};
    } else {
        UpdateCamera(&camera, CAMERA_FREE);
    }

    BeginDrawing();
    ClearBackground(Color{243, 245, 247, 255});

    BeginMode3D(camera);
    DrawWarehouse(simulation);
    EndMode3D();

    DrawHud(simulation);
    EndDrawing();
}
