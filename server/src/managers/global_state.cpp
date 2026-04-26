#include "managers/global_state.hpp"

SystemSnapshot GlobalState::snapshot() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return snapshot_;
}

void GlobalState::updateSystemState(SystemState state) {
    std::lock_guard<std::mutex> lock(mutex_);
    snapshot_.systemState = state;
}

void GlobalState::updatePower(const PowerState& power) {
    std::lock_guard<std::mutex> lock(mutex_);
    snapshot_.power = power;
}

void GlobalState::updateMotor(const MotorState& motor) {
    std::lock_guard<std::mutex> lock(mutex_);
    snapshot_.motor = motor;
}

void GlobalState::updateGouvernes(const GouvState& gouvernes) {
    std::lock_guard<std::mutex> lock(mutex_);
    snapshot_.gouvernes = gouvernes;
}

void GlobalState::updateSensors(const SensorState& sensors) {
    std::lock_guard<std::mutex> lock(mutex_);
    snapshot_.sensors = sensors;
}

void GlobalState::setCanStatus(bool channel0Ok, bool channel1Ok) {
    std::lock_guard<std::mutex> lock(mutex_);
    snapshot_.canChannel0Ok = channel0Ok;
    snapshot_.canChannel1Ok = channel1Ok;
}

void GlobalState::setShutdownRequested(bool value) {
    std::lock_guard<std::mutex> lock(mutex_);
    snapshot_.shutdownRequested = value;
}

void GlobalState::setEmergencyActive(bool value) {
    std::lock_guard<std::mutex> lock(mutex_);
    snapshot_.emergencyActive = value;
}
