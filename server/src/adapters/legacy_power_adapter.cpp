#include "adapters/legacy_power_adapter.hpp"

#ifndef CR_FUNC_OK
#define CR_FUNC_OK 0
#endif

LegacyPowerAdapter::LegacyPowerAdapter(LegacyContext& context)
    : context_(context) {}

bool LegacyPowerAdapter::setAlimentation240(std::uint8_t value) {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return SetAlimentation240(context_.raw(), value) == CR_FUNC_OK;
}

bool LegacyPowerAdapter::set480V(std::uint8_t value) {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return Set480V(context_.raw(), CONTACTEUR_TRAME_INDEXWRITE_BYTE0_VALUE, value) == CR_FUNC_OK;
}

bool LegacyPowerAdapter::setSafety(std::uint8_t index, std::uint8_t value) {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return SetSAFETY(context_.raw(), index, value) == CR_FUNC_OK;
}

bool LegacyPowerAdapter::setKcapa(std::uint8_t index, std::uint8_t value) {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return SetKCAPA(context_.raw(), index, value) == CR_FUNC_OK;
}

bool LegacyPowerAdapter::setPuissance(std::uint8_t index, std::uint8_t value) {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return SetPuissance(context_.raw(), index, value) == CR_FUNC_OK;
}

bool LegacyPowerAdapter::setShutdownLine(std::uint8_t index, std::uint8_t value) {
    std::lock_guard<std::mutex> lock(context_.mutex());
    return SetLigneShutDown(context_.raw(), index, value) == CR_FUNC_OK;
}
