#pragma once

#include "Simulation.h"

#include <raylib.h>

class Renderer {
public:
    void Draw(const Simulation& simulation, Camera3D& camera, bool topDown) const;

private:
    void DrawWarehouse(const Simulation& simulation) const;
    void DrawHud(const Simulation& simulation) const;
    Color RobotColor(int id) const;
};
