#ifndef COMMUNICAN_LEGACY_SENSOR_ADAPTER_HPP
#define COMMUNICAN_LEGACY_SENSOR_ADAPTER_HPP

#include "adapters/legacy_context.hpp"

extern "C" {
#include "DataValidation.h"
}

/**
 * LegacySensorAdapter
 * -------------------
 * Placeholder bridge for existing DataValidation / sensor decoding functions.
 *
 * The actual receive loop should call the legacy validation functions when a CAN frame arrives.
 * This adapter centralizes the future integration point for pressure, temperature, navigation,
 * battery and water-presence sensors.
 */
class LegacySensorAdapter {
public:
    explicit LegacySensorAdapter(LegacyContext& context);

    // Snapshot-style pull from stDataCommon. Fill these once exact fields are confirmed.
    bool refreshFromLegacyState();

private:
    LegacyContext& context_;
};

#endif
