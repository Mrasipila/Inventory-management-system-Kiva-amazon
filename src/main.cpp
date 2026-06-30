#include "Renderer.h"
#include "Simulation.h"

#include <raylib.h>

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1400, 900, "Factory Inventory Management - Goods-to-Person raylib MVP");
    SetTargetFPS(60);

    Camera3D camera{};
    camera.position = {20.0f, 34.0f, 28.0f};
    camera.target = {0.0f, 0.0f, 0.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Simulation simulation;
    Renderer renderer;
    bool topDown = false;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsKeyPressed(KEY_R)) {
            simulation.Reset();
        }
        if (IsKeyPressed(KEY_TAB)) {
            topDown = !topDown;
        }

        simulation.Update(dt);
        renderer.Draw(simulation, camera, topDown);
    }

    CloseWindow();
    return 0;
}
