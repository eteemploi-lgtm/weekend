#include "adapters/legacy_gouv_adapter.hpp"

#ifndef CR_FUNC_OK
#define CR_FUNC_OK 0
#endif

LegacyGouvAdapter::LegacyGouvAdapter(LegacyContext& context, int channel)
    : context_(context), channel_(channel) {}

bool LegacyGouvAdapter::configureAll() {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return SetConfigurationAllGouvernes(context_.raw(), channel_) == CR_FUNC_OK;
}

bool LegacyGouvAdapter::setOperationalAll() {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return SetOperationnelAllGouvernes(context_.raw(), channel_) == CR_FUNC_OK;
}

bool LegacyGouvAdapter::startHomingAll() {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return SetStartHMMAllgouvernes(context_.raw(), channel_) == CR_FUNC_OK;
}

bool LegacyGouvAdapter::configureOne(int gouverneIndex) {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return SetConfigurationGouverne(context_.raw(), channel_, gouverneIndex) == CR_FUNC_OK;
}

bool LegacyGouvAdapter::setTargetPosition(std::uint32_t nodeId, std::int32_t position) {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return SetSDOTargetPositionGouverne(context_.raw(), nodeId, channel_, position) == CR_FUNC_OK;
}

bool LegacyGouvAdapter::setRxPdoTargetPosition(std::uint8_t nodeId, std::int32_t position) {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return SetRxPDOTargetPositionGouverne(context_.raw(), nodeId, channel_, position) == CR_FUNC_OK;
}
