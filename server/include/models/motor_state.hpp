#ifndef COMMUNICAN_MOTOR_STATE_HPP
#define COMMUNICAN_MOTOR_STATE_HPP

#include <cstdint>

enum class MotorLifecycleState {
    Unknown = 0,
    Configured,
    CommunicationEnabled,
    Ready,
    OperationEnabled,
    Running,
    Stopped,
    Fault
};

struct MotorState {
    bool online = false;
    bool configured = false;
    bool enabled = false;
    bool fault = false;

    std::int32_t targetRpm = 0;
    std::int32_t measuredRpm = 0;
    std::int32_t temperatureC = 0;

    MotorLifecycleState lifecycle = MotorLifecycleState::Unknown;
};

#endif
