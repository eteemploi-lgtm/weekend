#include "models/json_serialization.hpp"
#include <sstream>

static const char* systemStateToString(SystemState state) {
    switch (state) {
        case SystemState::Init: return "INIT";
        case SystemState::PowerOff: return "POWER_OFF";
        case SystemState::PoweringOn: return "POWERING_ON";
        case SystemState::WaitingInterlocks: return "WAITING_INTERLOCKS";
        case SystemState::Configuring: return "CONFIGURING";
        case SystemState::Homing: return "HOMING";
        case SystemState::Ready: return "READY";
        case SystemState::Running: return "RUNNING";
        case SystemState::Stopping: return "STOPPING";
        case SystemState::Error: return "ERROR";
        case SystemState::EmergencyStop: return "EMERGENCY_STOP";
        default: return "UNKNOWN";
    }
}

std::string toJson(const SystemSnapshot& s) {
    std::ostringstream os;

    os << "{";
    os << "\"systemState\":\"" << systemStateToString(s.systemState) << "\",";
    os << "\"can\":{\"channel0\":" << (s.canChannel0Ok ? "true" : "false")
       << ",\"channel1\":" << (s.canChannel1Ok ? "true" : "false") << "},";

    os << "\"power\":{";
    os << "\"alimentation240On\":" << (s.power.alimentation240On ? "true" : "false") << ",";
    os << "\"alimentation480On\":" << (s.power.alimentation480On ? "true" : "false") << ",";
    os << "\"safetyOn\":" << (s.power.safetyOn ? "true" : "false") << ",";
    os << "\"kcapaOn\":" << (s.power.kcapaOn ? "true" : "false") << ",";
    os << "\"puissanceOn\":" << (s.power.puissanceOn ? "true" : "false") << ",";
    os << "\"shutdownLineActive\":" << (s.power.shutdownLineActive ? "true" : "false");
    os << "},";

    os << "\"motor\":{";
    os << "\"online\":" << (s.motor.online ? "true" : "false") << ",";
    os << "\"configured\":" << (s.motor.configured ? "true" : "false") << ",";
    os << "\"enabled\":" << (s.motor.enabled ? "true" : "false") << ",";
    os << "\"fault\":" << (s.motor.fault ? "true" : "false") << ",";
    os << "\"targetRpm\":" << s.motor.targetRpm << ",";
    os << "\"measuredRpm\":" << s.motor.measuredRpm << ",";
    os << "\"temperatureC\":" << s.motor.temperatureC;
    os << "},";

    os << "\"sensors\":{";
    os << "\"waterPresence\":" << (s.sensors.waterPresence ? "true" : "false");
    os << "}";

    os << "}";
    return os.str();
}
