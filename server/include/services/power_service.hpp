#ifndef COMMUNICAN_POWER_SERVICE_HPP
#define COMMUNICAN_POWER_SERVICE_HPP

#include "adapters/legacy_power_adapter.hpp"
#include "managers/global_state.hpp"

class PowerService {
public:
    PowerService(LegacyPowerAdapter& adapter, GlobalState& state);

    bool startup();
    bool shutdownNominal();
    bool emergencyStop();

private:
    LegacyPowerAdapter& adapter_;
    GlobalState& state_;
    PowerState powerState_{};
};

#endif
