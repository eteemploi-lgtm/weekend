#ifndef COMMUNICAN_LEGACY_GOUV_ADAPTER_HPP
#define COMMUNICAN_LEGACY_GOUV_ADAPTER_HPP

#include <cstdint>
#include "adapters/legacy_context.hpp"

extern "C" {
#include "gouvernes.h"
}

/**
 * LegacyGouvAdapter
 * -----------------
 * Thin C++ bridge over the existing C gouverne functions.
 */
class LegacyGouvAdapter {
public:
    explicit LegacyGouvAdapter(LegacyContext& context, int channel);

    bool configureAll();
    bool setOperationalAll();
    bool startHomingAll();

    bool configureOne(int gouverneIndex);
    bool setTargetPosition(std::uint32_t nodeId, std::int32_t position);
    bool setRxPdoTargetPosition(std::uint8_t nodeId, std::int32_t position);

private:
    LegacyContext& context_;
    int channel_;
};

#endif
