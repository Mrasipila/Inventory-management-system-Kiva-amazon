#include "Robot.h"

Robot::Robot(int id, GridPos startCell)
    : id_{id}, cell_{startCell}, homeCell_{startCell} {}

int Robot::GetId() const {
    return id_;
}

const GridPos& Robot::GetCell() const {
    return cell_;
}

void Robot::SetCell(const GridPos& cell) {
    cell_ = cell;
}

Robot::State Robot::GetState() const {
    return state_;
}

void Robot::SetState(State state) {
    state_ = state;
}

int Robot::GetActiveTaskId() const {
    return activeTaskId_;
}

void Robot::SetActiveTaskId(int taskId) {
    activeTaskId_ = taskId;
}

int Robot::GetCarriedPodId() const {
    return carriedPodId_;
}

void Robot::SetCarriedPodId(int podId) {
    carriedPodId_ = podId;
}

const GridPos& Robot::GetHomeCell() const {
    return homeCell_;
}

void Robot::SetHomeCell(const GridPos& cell) {
    homeCell_ = cell;
}

std::deque<GridPos>& Robot::GetPath() {
    return path_;
}

const std::deque<GridPos>& Robot::GetPath() const {
    return path_;
}

void Robot::ClearPath() {
    path_.clear();
}

void Robot::SetPath(std::deque<GridPos> path) {
    path_ = std::move(path);
}

float Robot::GetMoveAccumulator() const {
    return moveAccumulator_;
}

void Robot::AddMoveAccumulator(float value) {
    moveAccumulator_ += value;
}

void Robot::ResetMoveAccumulator() {
    moveAccumulator_ = 0.0f;
}

float Robot::GetSpeedCellsPerSecond() const {
    return speedCellsPerSecond_;
}

void Robot::SetSpeedCellsPerSecond(float value) {
    speedCellsPerSecond_ = value;
}

float Robot::GetDistanceTravelled() const {
    return distanceTravelled_;
}

void Robot::AddDistanceTravelled(float value) {
    distanceTravelled_ += value;
}

void Robot::ResetDistanceTravelled() {
    distanceTravelled_ = 0.0f;
}

float Robot::GetBlockedTime() const {
    return blockedTime_;
}

void Robot::AddBlockedTime(float value) {
    blockedTime_ += value;
}

void Robot::ResetBlockedTime() {
    blockedTime_ = 0.0f;
}

float Robot::GetServiceTime() const {
    return serviceTime_;
}

void Robot::AddServiceTime(float value) {
    serviceTime_ += value;
}

void Robot::ResetServiceTime() {
    serviceTime_ = 0.0f;
}

bool Robot::IsIdle() const {
    return state_ == State::Idle;
}
