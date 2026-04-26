#ifndef COMMUNICAN_SYSTEM_STATE_MACHINE_HPP
#define COMMUNICAN_SYSTEM_STATE_MACHINE_HPP

#include "services/motor_service.hpp"
#include "services/gouv_service.hpp"
#include "services/power_service.hpp"
#include "services/sensor_service.hpp"
#include "managers/global_state.hpp"

class SystemStateMachine {
public:
    SystemStateMachine(
        PowerService& power,
        MotorService& motor,
        GouvService& gouv,
        SensorService& sensors,
        GlobalState& state
    );

    bool startup();
    bool startRun(int rpm);
    bool stop();
    bool emergencyStop();

private:
    PowerService& power_;
    MotorService& motor_;
    GouvService& gouv_;
    SensorService& sensors_;
    GlobalState& state_;
};

#endif
