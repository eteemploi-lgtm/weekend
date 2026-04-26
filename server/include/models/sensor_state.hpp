#ifndef COMMUNICAN_SENSOR_STATE_HPP
#define COMMUNICAN_SENSOR_STATE_HPP

#include <array>

struct PressureSensorState {
    bool online = false;
    double pressure = 0.0;
    double temperature = 0.0;
};

struct NavigationSensorState {
    bool accOnline = false;
    bool gyroOnline = false;

    double accX = 0.0;
    double accY = 0.0;
    double accZ = 0.0;

    double gyroX = 0.0;
    double gyroY = 0.0;
    double gyroZ = 0.0;
};

struct TemperatureSensorState {
    bool online = false;
    double temperatureC = 0.0;
};

struct SensorState {
    PressureSensorState pressure;
    NavigationSensorState navigation;
    std::array<TemperatureSensorState, 4> temperatures{};
    bool waterPresence = false;
};

#endif
