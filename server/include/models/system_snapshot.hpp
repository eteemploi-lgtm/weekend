#ifndef COMMUNICAN_SYSTEM_SNAPSHOT_HPP
#define COMMUNICAN_SYSTEM_SNAPSHOT_HPP

#include "models/motor_state.hpp"
#include "models/gouv_state.hpp"
#include "models/power_state.hpp"
#include "models/sensor_state.hpp"

enum class SystemState {
    Init = 0,
    PowerOff,
    PoweringOn,
    WaitingInterlocks,
    Configuring,
    Homing,
    Ready,
    Running,
    Stopping,
    Error,
    EmergencyStop
};

struct SystemSnapshot {
    SystemState systemState = SystemState::Init;

    bool canChannel0Ok = false;
    bool canChannel1Ok = false;
    bool shutdownRequested = false;
    bool emergencyActive = false;

    PowerState power;
    MotorState motor;
    GouvState gouvernes;
    SensorState sensors;
};

#endif
