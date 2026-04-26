#ifndef COMMUNICAN_MOTOR_SERVICE_HPP
#define COMMUNICAN_MOTOR_SERVICE_HPP

#include <cstdint>
#include "adapters/legacy_motor_adapter.hpp"
#include "managers/global_state.hpp"

/**
 * MotorService
 * ------------
 * Business-level motor operations.
 * API routes should call this service, not the legacy C functions.
 */
class MotorService {
public:
    MotorService(LegacyMotorAdapter& adapter, GlobalState& state);

    bool configure();
    bool start();
    bool stop();
    bool setTargetSpeed(std::int32_t rpm);

private:
    LegacyMotorAdapter& adapter_;
    GlobalState& state_;
    MotorState motorState_{};
};

#endif
