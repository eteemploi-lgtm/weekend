#ifndef COMMUNICAN_GOUV_STATE_HPP
#define COMMUNICAN_GOUV_STATE_HPP

#include <array>
#include <cstdint>

enum class GouvLifecycleState {
    Unknown = 0,
    Configured,
    Homing,
    HomingDone,
    Operational,
    Moving,
    Fault
};

struct SingleGouvState {
    std::uint32_t nodeId = 0;
    bool online = false;
    bool configured = false;
    bool homingDone = false;
    bool operational = false;
    bool fault = false;

    double positionDeg = 0.0;
    double targetPositionDeg = 0.0;

    GouvLifecycleState lifecycle = GouvLifecycleState::Unknown;
};

struct GouvState {
    std::array<SingleGouvState, 4> axes{};
};

#endif
