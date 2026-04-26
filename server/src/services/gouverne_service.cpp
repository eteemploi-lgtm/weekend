#include "services/gouv_service.hpp"

GouvService::GouvService(LegacyGouvAdapter& adapter, GlobalState& state)
    : adapter_(adapter), state_(state) {
    for (std::size_t i = 0; i < gouvState_.axes.size(); ++i) {
        gouvState_.axes[i].nodeId = static_cast<std::uint32_t>(i + 1);
    }
}

bool GouvService::configureAll() {
    const bool ok = adapter_.configureAll();

    for (auto& axis : gouvState_.axes) {
        axis.configured = ok;
        axis.fault = !ok;
        axis.lifecycle = ok ? GouvLifecycleState::Configured : GouvLifecycleState::Fault;
    }

    state_.updateGouvernes(gouvState_);
    return ok;
}

bool GouvService::startHomingAll() {
    const bool ok = adapter_.startHomingAll();

    for (auto& axis : gouvState_.axes) {
        axis.lifecycle = ok ? GouvLifecycleState::Homing : GouvLifecycleState::Fault;
        axis.fault = !ok;
    }

    state_.updateGouvernes(gouvState_);
    return ok;
}

bool GouvService::setOperationalAll() {
    const bool ok = adapter_.setOperationalAll();

    for (auto& axis : gouvState_.axes) {
        axis.operational = ok;
        axis.fault = !ok;
        axis.lifecycle = ok ? GouvLifecycleState::Operational : GouvLifecycleState::Fault;
    }

    state_.updateGouvernes(gouvState_);
    return ok;
}

bool GouvService::moveAllToRawPosition(std::int32_t position) {
    bool allOk = true;

    for (auto& axis : gouvState_.axes) {
        const bool ok = adapter_.setRxPdoTargetPosition(
            static_cast<std::uint8_t>(axis.nodeId),
            position
        );

        axis.fault = !ok;
        axis.lifecycle = ok ? GouvLifecycleState::Moving : GouvLifecycleState::Fault;
        axis.targetPositionDeg = static_cast<double>(position);

        allOk = allOk && ok;
    }

    state_.updateGouvernes(gouvState_);
    return allOk;
}
