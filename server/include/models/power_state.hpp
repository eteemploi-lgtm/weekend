#ifndef COMMUNICAN_POWER_STATE_HPP
#define COMMUNICAN_POWER_STATE_HPP

struct PowerState {
    bool alimentation240On = false;
    bool alimentation480On = false;
    bool safetyOn = false;
    bool kcapaOn = false;
    bool puissanceOn = false;
    bool shutdownLineActive = false;
};

#endif
