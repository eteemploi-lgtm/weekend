#include "adapters/legacy_motor_adapter.hpp"

#ifndef CR_FUNC_OK
#define CR_FUNC_OK 0
#endif

LegacyMotorAdapter::LegacyMotorAdapter(LegacyContext& context, int channel)
    : context_(context), channel_(channel) {}

bool LegacyMotorAdapter::configurePdo() {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return SetConfigurationPDOSmo(context_.raw(), channel_) == CR_FUNC_OK;
}

bool LegacyMotorAdapter::start() {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return SetStartMoteur(context_.raw(), channel_) == CR_FUNC_OK;
}

bool LegacyMotorAdapter::stop() {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return SetStopMoteur(context_.raw(), channel_) == CR_FUNC_OK;
}

bool LegacyMotorAdapter::setSpeedRpm(std::int32_t rpm) {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return SetSpeedMoteur(context_.raw(), channel_, rpm) == CR_FUNC_OK;
}

bool LegacyMotorAdapter::setOperational() {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return SetMoteurCanEtatOperationel(context_.raw(), channel_) == CR_FUNC_OK;
}

bool LegacyMotorAdapter::resetControlWord() {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return SetControlWordSmoReset(context_.raw(), channel_) == CR_FUNC_OK;
}
