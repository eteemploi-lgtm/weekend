#include "services/motor_service.hpp"

MotorService::MotorService(LegacyMotorAdapter& adapter, GlobalState& state)
    : adapter_(adapter), state_(state) {}

bool MotorService::configure() {
    const bool ok = adapter_.configurePdo();

    motorState_.configured = ok;
    motorState_.fault = !ok;
    motorState_.lifecycle = ok ? MotorLifecycleState::Configured : MotorLifecycleState::Fault;

    state_.updateMotor(motorState_);
    return ok;
}

bool MotorService::start() {
    if (!motorState_.configured) {
        if (!configure()) {
            return false;
        }
    }

    const bool ok = adapter_.start();

    motorState_.enabled = ok;
    motorState_.online = ok;
    motorState_.fault = !ok;
    motorState_.lifecycle = ok ? MotorLifecycleState::OperationEnabled : MotorLifecycleState::Fault;

    state_.updateMotor(motorState_);
    return ok;
}

bool MotorService::stop() {
    const bool ok = adapter_.stop();

    motorState_.enabled = false;
    motorState_.targetRpm = 0;
    motorState_.fault = !ok;
    motorState_.lifecycle = ok ? MotorLifecycleState::Stopped : MotorLifecycleState::Fault;

    state_.updateMotor(motorState_);
    return ok;
}

bool MotorService::setTargetSpeed(std::int32_t rpm) {
    if (!motorState_.enabled) {
        return false;
    }

    const bool ok = adapter_.setSpeedRpm(rpm);

    if (ok) {
        motorState_.targetRpm = rpm;
        motorState_.lifecycle = rpm == 0 ? MotorLifecycleState::OperationEnabled : MotorLifecycleState::Running;
    } else {
        motorState_.fault = true;
        motorState_.lifecycle = MotorLifecycleState::Fault;
    }

    state_.updateMotor(motorState_);
    return ok;
}
