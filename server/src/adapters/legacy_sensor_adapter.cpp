#include "adapters/legacy_sensor_adapter.hpp"

LegacySensorAdapter::LegacySensorAdapter(LegacyContext& context)
    : context_(context) {}

bool LegacySensorAdapter::refreshFromLegacyState() {
    std::lock_guard<std::mutex> lock(context_.mutex());

    /*
     * The legacy receive thread likely already updates stDataCommon through
     * validateReadCapteurPression, validateReadNavigationAcc, validateReadNavigationGyro,
     * validateReadInfosbat, etc.
     *
     * Once the exact stDataCommon fields are confirmed, copy them into C++ models here.
     */
    return true;
}
