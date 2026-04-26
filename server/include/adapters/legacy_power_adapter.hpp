#ifndef COMMUNICAN_LEGACY_POWER_ADAPTER_HPP
#define COMMUNICAN_LEGACY_POWER_ADAPTER_HPP

#include <cstdint>
#include "adapters/legacy_context.hpp"

extern "C" {
#include "contacteurs.h"
}

/**
 * LegacyPowerAdapter
 * ------------------
 * Bridge over legacy contactor / safety / power functions.
 */
class LegacyPowerAdapter {
public:
    explicit LegacyPowerAdapter(LegacyContext& context);

    bool setAlimentation240(std::uint8_t value);
    bool set480V(std::uint8_t value);
    bool setSafety(std::uint8_t index, std::uint8_t value);
    bool setKcapa(std::uint8_t index, std::uint8_t value);
    bool setPuissance(std::uint8_t index, std::uint8_t value);
    bool setShutdownLine(std::uint8_t index, std::uint8_t value);

private:
    LegacyContext& context_;
};

#endif
