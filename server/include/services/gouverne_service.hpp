#ifndef COMMUNICAN_GOUV_SERVICE_HPP
#define COMMUNICAN_GOUV_SERVICE_HPP

#include <cstdint>
#include "adapters/legacy_gouv_adapter.hpp"
#include "managers/global_state.hpp"

class GouvService {
public:
    GouvService(LegacyGouvAdapter& adapter, GlobalState& state);

    bool configureAll();
    bool startHomingAll();
    bool setOperationalAll();
    bool moveAllToRawPosition(std::int32_t position);

private:
    LegacyGouvAdapter& adapter_;
    GlobalState& state_;
    GouvState gouvState_{};
};

#endif
