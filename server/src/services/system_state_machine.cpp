#include "services/system_state_machine.hpp"

SystemStateMachine::SystemStateMachine(
    PowerService& power,
    MotorService& motor,
    GouvService& gouv,
    SensorService& sensors,
    GlobalState& state
)
    : power_(power),
      motor_(motor),
      gouv_(gouv),
      sensors_(sensors),
      state_(state) {}

bool SystemStateMachine::startup() {
    state_.updateSystemState(SystemState::PoweringOn);
    if (!power_.startup()) {
        state_.updateSystemState(SystemState::Error);
        return false;
    }

    state_.updateSystemState(SystemState::Configuring);

    if (!motor_.configure()) {
        state_.updateSystemState(SystemState::Error);
        return false;
    }

    if (!gouv_.configureAll()) {
        state_.updateSystemState(SystemState::Error);
        return false;
    }

    state_.updateSystemState(SystemState::Homing);

    if (!gouv_.startHomingAll()) {
        state_.updateSystemState(SystemState::Error);
        return false;
    }

    if (!gouv_.setOperationalAll()) {
        state_.updateSystemState(SystemState::Error);
        return false;
    }

    if (!motor_.start()) {
        state_.updateSystemState(SystemState::Error);
        return false;
    }

    state_.updateSystemState(SystemState::Ready);
    return true;
}

bool SystemStateMachine::startRun(int rpm) {
    state_.updateSystemState(SystemState::Running);
    if (!motor_.setTargetSpeed(rpm)) {
        state_.updateSystemState(SystemState::Error);
        return false;
    }
    return true;
}

bool SystemStateMachine::stop() {
    state_.updateSystemState(SystemState::Stopping);

    const bool motorOk = motor_.stop();
    const bool powerOk = power_.shutdownNominal();

    state_.updateSystemState((motorOk && powerOk) ? SystemState::PowerOff : SystemState::Error);
    return motorOk && powerOk;
}

bool SystemStateMachine::emergencyStop() {
    state_.updateSystemState(SystemState::EmergencyStop);

    const bool motorOk = motor_.stop();
    const bool powerOk = power_.emergencyStop();

    return motorOk && powerOk;
}
