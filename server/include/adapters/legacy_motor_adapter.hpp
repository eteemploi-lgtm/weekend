#ifndef COMMUNICAN_LEGACY_MOTOR_ADAPTER_HPP
#define COMMUNICAN_LEGACY_MOTOR_ADAPTER_HPP

#include <cstdint>
#include "adapters/legacy_context.hpp"

extern "C" {
#include "moteur.h"
}

/**
 * LegacyMotorAdapter
 * ------------------
 * Thin C++ bridge over the existing C motor functions.
 *
 * This class must stay close to the legacy API.
 * Business sequencing belongs in MotorService, not here.
 */
class LegacyMotorAdapter {
public:
    explicit LegacyMotorAdapter(LegacyContext& context, int channel);

    bool configurePdo();
    bool start();
    bool stop();
    bool setSpeedRpm(std::int32_t rpm);
    bool setOperational();
    bool resetControlWord();

private:
    LegacyContext& context_;
    int channel_;
};

#endif
