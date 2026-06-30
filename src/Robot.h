#pragma once

#include "GridPos.h"

#include <deque>

class Robot {
public:
    enum class State {
        Idle,
        ToPod,
        ToStation,
        PickingAtStation,
        ReturningHome
    };

    Robot() = default;
    Robot(int id, GridPos startCell);

    int GetId() const;
    const GridPos& GetCell() const;
    void SetCell(const GridPos& cell);
    State GetState() const;
    void SetState(State state);
    int GetActiveTaskId() const;
    void SetActiveTaskId(int taskId);
    int GetCarriedPodId() const;
    void SetCarriedPodId(int podId);
    const GridPos& GetHomeCell() const;
    void SetHomeCell(const GridPos& cell);
    std::deque<GridPos>& GetPath();
    const std::deque<GridPos>& GetPath() const;
    void ClearPath();
    void SetPath(std::deque<GridPos> path);
    float GetMoveAccumulator() const;
    void AddMoveAccumulator(float value);
    void ResetMoveAccumulator();
    float GetSpeedCellsPerSecond() const;
    void SetSpeedCellsPerSecond(float value);
    float GetDistanceTravelled() const;
    void AddDistanceTravelled(float value);
    void ResetDistanceTravelled();
    float GetBlockedTime() const;
    void AddBlockedTime(float value);
    void ResetBlockedTime();
    float GetServiceTime() const;
    void AddServiceTime(float value);
    void ResetServiceTime();
    bool IsIdle() const;

private:
    int id_{0};
    GridPos cell_{};
    GridPos homeCell_{};
    State state_{State::Idle};
    std::deque<GridPos> path_;
    float speedCellsPerSecond_{3.0f};
    float moveAccumulator_{0.0f};
    int activeTaskId_{-1};
    int carriedPodId_{-1};
    float distanceTravelled_{0.0f};
    float blockedTime_{0.0f};
    float serviceTime_{0.0f};
};
