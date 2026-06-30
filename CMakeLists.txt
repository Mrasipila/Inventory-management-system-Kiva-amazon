cmake_minimum_required(VERSION 3.20)
project(warehouse_sim_raylib CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 5.5
)

FetchContent_MakeAvailable(raylib)

add_executable(warehouse_sim
  src/GridPos.cpp
  src/Order.cpp
  src/OrderGenerator.cpp
  src/PathFinder.cpp
  src/Pod.cpp
  src/Renderer.cpp
  src/Robot.cpp
  src/Simulation.cpp
  src/Task.cpp
  src/TaskDispatcher.cpp
  src/WarehouseMap.cpp
  src/main.cpp
)

target_link_libraries(warehouse_sim PRIVATE raylib)

if (WIN32)
  target_link_libraries(warehouse_sim PRIVATE winmm)
endif()
