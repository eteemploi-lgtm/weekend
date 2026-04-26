#ifndef COMMUNICAN_SENSOR_SERVICE_HPP
#define COMMUNICAN_SENSOR_SERVICE_HPP

#include "adapters/legacy_sensor_adapter.hpp"
#include "managers/global_state.hpp"

class SensorService {
public:
    SensorService(LegacySensorAdapter& adapter, GlobalState& state);

    bool refresh();

private:
    LegacySensorAdapter& adapter_;
    GlobalState& state_;
    SensorState sensorState_{};
};

#endif
