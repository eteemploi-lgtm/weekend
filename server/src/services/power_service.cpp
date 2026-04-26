#include "services/power_service.hpp"

PowerService::PowerService(LegacyPowerAdapter& adapter, GlobalState& state)
    : adapter_(adapter), state_(state) {}

bool PowerService::startup() {
    bool ok = true;

#ifdef CONTACTEUR_ALIMENTATION240ON_BYTE0_VALUE
    ok = ok && adapter_.setAlimentation240(CONTACTEUR_ALIMENTATION240ON_BYTE0_VALUE);
#else
    ok = ok && adapter_.setAlimentation240(0x0B);
#endif
    powerState_.alimentation240On = ok;

#ifdef CONTACTEUR_480V_VALEUR_ON
    ok = ok && adapter_.set480V(CONTACTEUR_480V_VALEUR_ON);
#else
    ok = ok && adapter_.set480V(0xFF);
#endif
    powerState_.alimentation480On = ok;

#ifdef CONTACTEUR_KCAPA_VALEUR_ON
    ok = ok && adapter_.setKcapa(CONTACTEUR_TRAME_INDEXWRITE_BYTE0_VALUE, CONTACTEUR_KCAPA_VALEUR_ON);
    powerState_.kcapaOn = ok;
#endif

#ifdef CONTACTEUR_PUISSANCE_VALEUR_ON
    ok = ok && adapter_.setPuissance(CONTACTEUR_TRAME_INDEXWRITE_BYTE0_VALUE, CONTACTEUR_PUISSANCE_VALEUR_ON);
    powerState_.puissanceOn = ok;
#endif

#ifdef CONTACTEUR_KCAPA_VALEUR_OFF
    ok = ok && adapter_.setKcapa(CONTACTEUR_TRAME_INDEXWRITE_BYTE0_VALUE, CONTACTEUR_KCAPA_VALEUR_OFF);
    powerState_.kcapaOn = false;
#endif

#ifdef CONTACTEUR_SAFETY_VALEUR_ON
    ok = ok && adapter_.setSafety(CONTACTEUR_TRAME_INDEXWRITE_BYTE0_VALUE, CONTACTEUR_SAFETY_VALEUR_ON);
    powerState_.safetyOn = ok;
#endif

    state_.updatePower(powerState_);
    return ok;
}

bool PowerService::shutdownNominal() {
    bool ok = true;

#ifdef CONTACTEUR_SAFETY_VALEUR_OFF
    ok = ok && adapter_.setSafety(CONTACTEUR_TRAME_INDEXWRITE_BYTE0_VALUE, CONTACTEUR_SAFETY_VALEUR_OFF);
#endif
    powerState_.safetyOn = false;

#ifdef CONTACTEUR_LIGNESHD_ECRIRE_VALUE
    ok = ok && adapter_.setShutdownLine(CONTACTEUR_TRAME_INDEXWRITE_BYTE2_VALUE, CONTACTEUR_LIGNESHD_ECRIRE_VALUE);
#endif
    powerState_.shutdownLineActive = true;

#ifdef CONTACTEUR_480V_VALEUR_OFF
    ok = ok && adapter_.set480V(CONTACTEUR_480V_VALEUR_OFF);
#endif
    powerState_.alimentation480On = false;

#ifdef CONTACTEUR_ALIMENTATION240OFF_BYTE0_VALUE
    ok = ok && adapter_.setAlimentation240(CONTACTEUR_ALIMENTATION240OFF_BYTE0_VALUE);
#else
    ok = ok && adapter_.setAlimentation240(0x0A);
#endif
    powerState_.alimentation240On = false;

    state_.updatePower(powerState_);
    return ok;
}

bool PowerService::emergencyStop() {
    state_.setEmergencyActive(true);
    return shutdownNominal();
}
