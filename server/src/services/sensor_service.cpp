#include "services/sensor_service.hpp"

SensorService::SensorService(LegacySensorAdapter& adapter, GlobalState& state)
    : adapter_(adapter), state_(state) {}

bool SensorService::refresh() {
    const bool ok = adapter_.refreshFromLegacyState();

    /*
     * TODO:
     * Copy real values from stDataCommon through LegacySensorAdapter once
     * exact fields are confirmed.
     */

    state_.updateSensors(sensorState_);
    return ok;
}
