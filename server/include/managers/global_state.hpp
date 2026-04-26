#ifndef COMMUNICAN_GLOBAL_STATE_HPP
#define COMMUNICAN_GLOBAL_STATE_HPP

#include <mutex>
#include "models/system_snapshot.hpp"

/**
 * GlobalState
 * -----------
 * Thread-safe supervision state for HTTP/WebSocket.
 *
 * Rule:
 * - API/WebSocket reads this.
 * - Services update this.
 * - Legacy C should not write this directly.
 */
class GlobalState {
public:
    SystemSnapshot snapshot() const;

    void updateSystemState(SystemState state);
    void updatePower(const PowerState& power);
    void updateMotor(const MotorState& motor);
    void updateGouvernes(const GouvState& gouvernes);
    void updateSensors(const SensorState& sensors);

    void setCanStatus(bool channel0Ok, bool channel1Ok);
    void setShutdownRequested(bool value);
    void setEmergencyActive(bool value);

private:
    mutable std::mutex mutex_;
    SystemSnapshot snapshot_{};
};

#endif
