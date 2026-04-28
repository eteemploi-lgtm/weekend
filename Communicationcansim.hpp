#ifndef COMMUNICAN_CAN_SIM_BUS_HPP
#define COMMUNICAN_CAN_SIM_BUS_HPP

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include "sim/can_sim_frame.hpp"

class CanSimBus {
public:
    CanSimBus() = default;
    ~CanSimBus();

    void start();
    void stop();

    void send(const CanSimFrame& frame);
    bool receive(CanSimFrame& outFrame);

private:
    void loop();
    void pushRx(const CanSimFrame& frame);

    std::atomic<bool> running_{false};
    std::thread worker_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<CanSimFrame> txQueue_;
    std::queue<CanSimFrame> rxQueue_;

    int tick_ = 0;
    int motorRpm_ = 0;
    int targetMotorRpm_ = 0;
};

#endif



#include "sim/can_sim_bus.hpp"
#include <chrono>
#include <iostream>

CanSimBus::~CanSimBus() {
    stop();
}

void CanSimBus::start() {
    if (running_) return;
    running_ = true;
    worker_ = std::thread(&CanSimBus::loop, this);
}

void CanSimBus::stop() {
    if (!running_) return;
    running_ = false;
    cv_.notify_all();
    if (worker_.joinable()) worker_.join();
}

void CanSimBus::send(const CanSimFrame& frame) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        txQueue_.push(frame);
    }
    cv_.notify_all();
}

bool CanSimBus::receive(CanSimFrame& outFrame) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (rxQueue_.empty()) return false;
    outFrame = rxQueue_.front();
    rxQueue_.pop();
    return true;
}

void CanSimBus::pushRx(const CanSimFrame& frame) {
    std::lock_guard<std::mutex> lock(mutex_);
    rxQueue_.push(frame);
}

void CanSimBus::loop() {
    using namespace std::chrono_literals;

    while (running_) {
        std::queue<CanSimFrame> localTx;

        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait_for(lock, 50ms, [&] { return !txQueue_.empty() || !running_; });
            while (!txQueue_.empty()) {
                localTx.push(txQueue_.front());
                txQueue_.pop();
            }
        }

        while (!localTx.empty()) {
            auto frame = localTx.front();
            localTx.pop();

            std::cout << "[CAN_SIM][TX] " << frame.toString() << "\n";

            if (frame.id == 0x201 && frame.dlc >= 4) {
                targetMotorRpm_ =
                    static_cast<int>(frame.data[0]) |
                    (static_cast<int>(frame.data[1]) << 8) |
                    (static_cast<int>(frame.data[2]) << 16) |
                    (static_cast<int>(frame.data[3]) << 24);

                CanSimFrame ack;
                ack.id = 0x181;
                ack.dlc = 8;
                ack.data[4] = 42;
                pushRx(ack);
            }

            if (frame.id >= 0x401 && frame.id <= 0x404 && frame.dlc >= 4) {
                CanSimFrame gouv;
                gouv.id = 0x381 + (frame.id - 0x401);
                gouv.dlc = 8;
                gouv.data = frame.data;
                gouv.data[4] = 1;
                pushRx(gouv);
            }
        }

        if (motorRpm_ < targetMotorRpm_) motorRpm_ += 50;
        if (motorRpm_ > targetMotorRpm_) motorRpm_ -= 50;

        tick_++;

        if (tick_ % 4 == 0) {
            CanSimFrame motor;
            motor.id = 0x181;
            motor.dlc = 8;
            motor.data[0] = motorRpm_ & 0xFF;
            motor.data[1] = (motorRpm_ >> 8) & 0xFF;
            motor.data[2] = (motorRpm_ >> 16) & 0xFF;
            motor.data[3] = (motorRpm_ >> 24) & 0xFF;
            motor.data[4] = 42;
            pushRx(motor);
        }

        if (tick_ % 6 == 0) {
            CanSimFrame pressure;
            pressure.id = 0x183;
            pressure.dlc = 8;
            pressure.data[0] = 100;
            pressure.data[1] = 0;
            pressure.data[2] = 25;
            pushRx(pressure);
        }

        if (tick_ % 8 == 0) {
            CanSimFrame nav;
            nav.id = 0x134;
            nav.dlc = 8;
            nav.data[0] = 1;
            nav.data[1] = 2;
            nav.data[2] = 3;
            pushRx(nav);
        }

        if (tick_ % 10 == 0) {
            CanSimFrame temp;
            temp.id = 0x371;
            temp.dlc = 8;
            temp.data[0] = 30;
            pushRx(temp);
        }
    }
}

ifndef COMMUNICAN_CAN_SIM_FRAME_HPP
#define COMMUNICAN_CAN_SIM_FRAME_HPP

#include <array>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

struct CanSimFrame {
    std::uint32_t id = 0;
    std::uint8_t dlc = 8;
    std::array<std::uint8_t, 8> data{};

    std::string toString() const {
        std::ostringstream os;
        os << "ID=0x" << std::hex << std::uppercase << id
           << " DLC=" << std::dec << static_cast<int>(dlc) << " DATA=[";
        for (int i = 0; i < dlc; ++i) {
            if (i) os << " ";
            os << "0x" << std::hex << std::uppercase << std::setw(2)
               << std::setfill('0') << static_cast<int>(data[i]);
        }
        os << "]";
        return os.str();
    }
};

#endif


ifndef COMMUNICAN_SIMULATED_DEVICE_BACKEND_HPP
#define COMMUNICAN_SIMULATED_DEVICE_BACKEND_HPP

#include <cstdint>
#include "models/system_snapshot.hpp"
#include "sim/can_sim_bus.hpp"

class SimulatedDeviceBackend {
public:
    explicit SimulatedDeviceBackend(CanSimBus& bus);

    bool powerStartup();
    bool powerShutdown();

    bool configureMotor();
    bool startMotor();
    bool stopMotor();
    bool setMotorSpeed(std::int32_t rpm);

    bool configureGouvernes();
    bool startHomingGouvernes();
    bool setGouvernesOperational();
    bool moveGouvernesRaw(std::int32_t rawPosition);

    void poll(SystemSnapshot& snapshot);

private:
    void sendInt32(std::uint32_t id, std::int32_t value);

    CanSimBus& bus_;
    bool motorConfigured_ = false;
    bool motorEnabled_ = false;
    bool gouvConfigured_ = false;
    bool gouvOperational_ = false;
    PowerState power_{};
};

#endif



include "sim/simulated_device_backend.hpp"
#include <iostream>

SimulatedDeviceBackend::SimulatedDeviceBackend(CanSimBus& bus) : bus_(bus) {}

void SimulatedDeviceBackend::sendInt32(std::uint32_t id, std::int32_t value) {
    CanSimFrame f;
    f.id = id;
    f.dlc = 8;
    f.data[0] = value & 0xFF;
    f.data[1] = (value >> 8) & 0xFF;
    f.data[2] = (value >> 16) & 0xFF;
    f.data[3] = (value >> 24) & 0xFF;
    bus_.send(f);
}

bool SimulatedDeviceBackend::powerStartup() {
    power_.alimentation240On = true;
    power_.alimentation480On = true;
    power_.safetyOn = true;
    power_.puissanceOn = true;
    return true;
}

bool SimulatedDeviceBackend::powerShutdown() {
    power_ = {};
    return true;
}

bool SimulatedDeviceBackend::configureMotor() {
    motorConfigured_ = true;
    return true;
}

bool SimulatedDeviceBackend::startMotor() {
    if (!motorConfigured_) configureMotor();
    motorEnabled_ = true;
    return true;
}

bool SimulatedDeviceBackend::stopMotor() {
    sendInt32(0x201, 0);
    motorEnabled_ = false;
    return true;
}

bool SimulatedDeviceBackend::setMotorSpeed(std::int32_t rpm) {
    if (!motorEnabled_) return false;
    sendInt32(0x201, rpm);
    return true;
}

bool SimulatedDeviceBackend::configureGouvernes() {
    gouvConfigured_ = true;
    return true;
}

bool SimulatedDeviceBackend::startHomingGouvernes() {
    return gouvConfigured_;
}

bool SimulatedDeviceBackend::setGouvernesOperational() {
    if (!gouvConfigured_) return false;
    gouvOperational_ = true;
    return true;
}

bool SimulatedDeviceBackend::moveGouvernesRaw(std::int32_t rawPosition) {
    if (!gouvOperational_) return false;
    sendInt32(0x401, rawPosition);
    sendInt32(0x402, rawPosition);
    sendInt32(0x403, rawPosition);
    sendInt32(0x404, rawPosition);
    return true;
}

void SimulatedDeviceBackend::poll(SystemSnapshot& s) {
    s.power = power_;
    s.motor.online = true;
    s.motor.configured = motorConfigured_;
    s.motor.enabled = motorEnabled_;

    CanSimFrame f;
    while (bus_.receive(f)) {
        std::cout << "[CAN_SIM][RX] " << f.toString() << "\n";

        if (f.id == 0x181) {
            std::int32_t rpm =
                static_cast<int>(f.data[0]) |
                (static_cast<int>(f.data[1]) << 8) |
                (static_cast<int>(f.data[2]) << 16) |
                (static_cast<int>(f.data[3]) << 24);
            s.motor.measuredRpm = rpm;
            s.motor.temperatureC = f.data[4];
        }

        if (f.id == 0x183) {
            s.sensors.pressure.online = true;
            s.sensors.pressure.pressure =
                static_cast<double>(static_cast<int>(f.data[0]) | (static_cast<int>(f.data[1]) << 8));
            s.sensors.pressure.temperature = f.data[2];
        }

        if (f.id == 0x134) {
            s.sensors.navigation.accOnline = true;
            s.sensors.navigation.accX = f.data[0];
            s.sensors.navigation.accY = f.data[1];
            s.sensors.navigation.accZ = f.data[2];
        }

        if (f.id == 0x371) {
            s.sensors.temperatures[0].online = true;
            s.sensors.temperatures[0].temperatureC = f.data[0];
        }

        if (f.id >= 0x381 && f.id <= 0x384) {
            auto index = static_cast<std::size_t>(f.id - 0x381);
            if (index < s.gouvernes.axes.size()) {
                s.gouvernes.axes[index].online = true;
                s.gouvernes.axes[index].operational = true;
                s.gouvernes.axes[index].positionDeg =
                    static_cast<double>(static_cast<int>(f.data[0]) | (static_cast<int>(f.data[1]) << 8));
            }
        }
    }
}





include <chrono>
#include <iostream>
#include <thread>

#include "models/json_serialization.hpp"
#include "models/system_snapshot.hpp"
#include "sim/can_sim_bus.hpp"
#include "sim/simulated_device_backend.hpp"

int main() {
    std::cout << "========== CommuniCAN CAN simulation test ==========\n";

    CanSimBus bus;
    SimulatedDeviceBackend backend(bus);
    SystemSnapshot snapshot;

    bus.start();

    snapshot.systemState = SystemState::PoweringOn;
    backend.powerStartup();

    snapshot.systemState = SystemState::Configuring;
    backend.configureMotor();
    backend.configureGouvernes();

    snapshot.systemState = SystemState::Homing;
    backend.startHomingGouvernes();
    backend.setGouvernesOperational();

    snapshot.systemState = SystemState::Running;
    backend.startMotor();
    backend.setMotorSpeed(1200);
    backend.moveGouvernesRaw(1500);

    for (int i = 0; i < 30; ++i) {
        backend.poll(snapshot);
        std::cout << toJson(snapshot) << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

    backend.stopMotor();
    backend.powerShutdown();
    backend.poll(snapshot);

    snapshot.systemState = SystemState::PowerOff;
    std::cout << toJson(snapshot) << "\n";

    bus.stop();

    std::cout << "[OK] CAN simulation test completed\n";
    return 0;
}







include "sim/simulated_device_backend.hpp"
#include <iostream>

SimulatedDeviceBackend::SimulatedDeviceBackend(CanSimBus& bus) : bus_(bus) {}

void SimulatedDeviceBackend::sendInt32(std::uint32_t id, std::int32_t value) {
    CanSimFrame f;
    f.id = id;
    f.dlc = 8;
    f.data[0] = value & 0xFF;
    f.data[1] = (value >> 8) & 0xFF;
    f.data[2] = (value >> 16) & 0xFF;
    f.data[3] = (value >> 24) & 0xFF;
    bus_.send(f);
}

bool SimulatedDeviceBackend::powerStartup() {
    power_.alimentation240On = true;
    power_.alimentation480On = true;
    power_.safetyOn = true;
    power_.puissanceOn = true;
    return true;
}

bool SimulatedDeviceBackend::powerShutdown() {
    power_ = {};
    return true;
}

bool SimulatedDeviceBackend::configureMotor() {
    motorConfigured_ = true;
    return true;
}

bool SimulatedDeviceBackend::startMotor() {
    if (!motorConfigured_) configureMotor();
    motorEnabled_ = true;
    return true;
}

bool SimulatedDeviceBackend::stopMotor() {
    sendInt32(0x201, 0);
    motorEnabled_ = false;
    return true;
}

bool SimulatedDeviceBackend::setMotorSpeed(std::int32_t rpm) {
    if (!motorEnabled_) return false;
    sendInt32(0x201, rpm);
    return true;
}

bool SimulatedDeviceBackend::configureGouvernes() {
    gouvConfigured_ = true;
    return true;
}

bool SimulatedDeviceBackend::startHomingGouvernes() {
    return gouvConfigured_;
}

bool SimulatedDeviceBackend::setGouvernesOperational() {
    if (!gouvConfigured_) return false;
    gouvOperational_ = true;
    return true;
}

bool SimulatedDeviceBackend::moveGouvernesRaw(std::int32_t rawPosition) {
    if (!gouvOperational_) return false;
    sendInt32(0x401, rawPosition);
    sendInt32(0x402, rawPosition);
    sendInt32(0x403, rawPosition);
    sendInt32(0x404, rawPosition);
    return true;
}

void SimulatedDeviceBackend::poll(SystemSnapshot& s) {
    s.power = power_;
    s.motor.online = true;
    s.motor.configured = motorConfigured_;
    s.motor.enabled = motorEnabled_;

    CanSimFrame f;
    while (bus_.receive(f)) {
        std::cout << "[CAN_SIM][RX] " << f.toString() << "\n";

        if (f.id == 0x181) {
            std::int32_t rpm =
                static_cast<int>(f.data[0]) |
                (static_cast<int>(f.data[1]) << 8) |
                (static_cast<int>(f.data[2]) << 16) |
                (static_cast<int>(f.data[3]) << 24);
            s.motor.measuredRpm = rpm;
            s.motor.temperatureC = f.data[4];
        }

        if (f.id == 0x183) {
            s.sensors.pressure.online = true;
            s.sensors.pressure.pressure =
                static_cast<double>(static_cast<int>(f.data[0]) | (static_cast<int>(f.data[1]) << 8));
            s.sensors.pressure.temperature = f.data[2];
        }

        if (f.id == 0x134) {
            s.sensors.navigation.accOnline = true;
            s.sensors.navigation.accX = f.data[0];
            s.sensors.navigation.accY = f.data[1];
            s.sensors.navigation.accZ = f.data[2];
        }

        if (f.id == 0x371) {
            s.sensors.temperatures[0].online = true;
            s.sensors.temperatures[0].temperatureC = f.data[0];
        }

        if (f.id >= 0x381 && f.id <= 0x384) {
            auto index = static_cast<std::size_t>(f.id - 0x381);
            if (index < s.gouvernes.axes.size()) {
                s.gouvernes.axes[index].online = true;
                s.gouvernes.axes[index].operational = true;
                s.gouvernes.axes[index].positionDeg =
                    static_cast<double>(static_cast<int>(f.data[0]) | (static_cast<int>(f.data[1]) << 8));
            }
        }
    }
}

include "sim/simulated_device_backend.hpp"
#include <iostream>

SimulatedDeviceBackend::SimulatedDeviceBackend(CanSimBus& bus) : bus_(bus) {}

void SimulatedDeviceBackend::sendInt32(std::uint32_t id, std::int32_t value) {
    CanSimFrame f;
    f.id = id;
    f.dlc = 8;
    f.data[0] = value & 0xFF;
    f.data[1] = (value >> 8) & 0xFF;
    f.data[2] = (value >> 16) & 0xFF;
    f.data[3] = (value >> 24) & 0xFF;
    bus_.send(f);
}

bool SimulatedDeviceBackend::powerStartup() {
    power_.alimentation240On = true;
    power_.alimentation480On = true;
    power_.safetyOn = true;
    power_.puissanceOn = true;
    return true;
}

bool SimulatedDeviceBackend::powerShutdown() {
    power_ = {};
    return true;
}

bool SimulatedDeviceBackend::configureMotor() {
    motorConfigured_ = true;
    return true;
}

bool SimulatedDeviceBackend::startMotor() {
    if (!motorConfigured_) configureMotor();
    motorEnabled_ = true;
    return true;
}

bool SimulatedDeviceBackend::stopMotor() {
    sendInt32(0x201, 0);
    motorEnabled_ = false;
    return true;
}

bool SimulatedDeviceBackend::setMotorSpeed(std::int32_t rpm) {
    if (!motorEnabled_) return false;
    sendInt32(0x201, rpm);
    return true;
}

bool SimulatedDeviceBackend::configureGouvernes() {
    gouvConfigured_ = true;
    return true;
}

bool SimulatedDeviceBackend::startHomingGouvernes() {
    return gouvConfigured_;
}

bool SimulatedDeviceBackend::setGouvernesOperational() {
    if (!gouvConfigured_) return false;
    gouvOperational_ = true;
    return true;
}

bool SimulatedDeviceBackend::moveGouvernesRaw(std::int32_t rawPosition) {
    if (!gouvOperational_) return false;
    sendInt32(0x401, rawPosition);
    sendInt32(0x402, rawPosition);
    sendInt32(0x403, rawPosition);
    sendInt32(0x404, rawPosition);
    return true;
}

void SimulatedDeviceBackend::poll(SystemSnapshot& s) {
    s.power = power_;
    s.motor.online = true;
    s.motor.configured = motorConfigured_;
    s.motor.enabled = motorEnabled_;

    CanSimFrame f;
    while (bus_.receive(f)) {
        std::cout << "[CAN_SIM][RX] " << f.toString() << "\n";

        if (f.id == 0x181) {
            std::int32_t rpm =
                static_cast<int>(f.data[0]) |
                (static_cast<int>(f.data[1]) << 8) |
                (static_cast<int>(f.data[2]) << 16) |
                (static_cast<int>(f.data[3]) << 24);
            s.motor.measuredRpm = rpm;
            s.motor.temperatureC = f.data[4];
        }

        if (f.id == 0x183) {
            s.sensors.pressure.online = true;
            s.sensors.pressure.pressure =
                static_cast<double>(static_cast<int>(f.data[0]) | (static_cast<int>(f.data[1]) << 8));
            s.sensors.pressure.temperature = f.data[2];
        }

        if (f.id == 0x134) {
            s.sensors.navigation.accOnline = true;
            s.sensors.navigation.accX = f.data[0];
            s.sensors.navigation.accY = f.data[1];
            s.sensors.navigation.accZ = f.data[2];
        }

        if (f.id == 0x371) {
            s.sensors.temperatures[0].online = true;
            s.sensors.temperatures[0].temperatureC = f.data[0];
        }

        if (f.id >= 0x381 && f.id <= 0x384) {
            auto index = static_cast<std::size_t>(f.id - 0x381);
            if (index < s.gouvernes.axes.size()) {
                s.gouvernes.axes[index].online = true;
                s.gouvernes.axes[index].operational = true;
                s.gouvernes.axes[index].positionDeg =
                    static_cast<double>(static_cast<int>(f.data[0]) | (static_cast<int>(f.data[1]) << 8));
            }
        }
    }
}




include "sim/simulated_device_backend.hpp"
#include <iostream>

SimulatedDeviceBackend::SimulatedDeviceBackend(CanSimBus& bus) : bus_(bus) {}

void SimulatedDeviceBackend::sendInt32(std::uint32_t id, std::int32_t value) {
    CanSimFrame f;
    f.id = id;
    f.dlc = 8;
    f.data[0] = value & 0xFF;
    f.data[1] = (value >> 8) & 0xFF;
    f.data[2] = (value >> 16) & 0xFF;
    f.data[3] = (value >> 24) & 0xFF;
    bus_.send(f);
}

bool SimulatedDeviceBackend::powerStartup() {
    power_.alimentation240On = true;
    power_.alimentation480On = true;
    power_.safetyOn = true;
    power_.puissanceOn = true;
    return true;
}

bool SimulatedDeviceBackend::powerShutdown() {
    power_ = {};
    return true;
}

bool SimulatedDeviceBackend::configureMotor() {
    motorConfigured_ = true;
    return true;
}

bool SimulatedDeviceBackend::startMotor() {
    if (!motorConfigured_) configureMotor();
    motorEnabled_ = true;
    return true;
}

bool SimulatedDeviceBackend::stopMotor() {
    sendInt32(0x201, 0);
    motorEnabled_ = false;
    return true;
}

bool SimulatedDeviceBackend::setMotorSpeed(std::int32_t rpm) {
    if (!motorEnabled_) return false;
    sendInt32(0x201, rpm);
    return true;
}

bool SimulatedDeviceBackend::configureGouvernes() {
    gouvConfigured_ = true;
    return true;
}

bool SimulatedDeviceBackend::startHomingGouvernes() {
    return gouvConfigured_;
}

bool SimulatedDeviceBackend::setGouvernesOperational() {
    if (!gouvConfigured_) return false;
    gouvOperational_ = true;
    return true;
}

bool SimulatedDeviceBackend::moveGouvernesRaw(std::int32_t rawPosition) {
    if (!gouvOperational_) return false;
    sendInt32(0x401, rawPosition);
    sendInt32(0x402, rawPosition);
    sendInt32(0x403, rawPosition);
    sendInt32(0x404, rawPosition);
    return true;
}

void SimulatedDeviceBackend::poll(SystemSnapshot& s) {
    s.power = power_;
    s.motor.online = true;
    s.motor.configured = motorConfigured_;
    s.motor.enabled = motorEnabled_;

    CanSimFrame f;
    while (bus_.receive(f)) {
        std::cout << "[CAN_SIM][RX] " << f.toString() << "\n";

        if (f.id == 0x181) {
            std::int32_t rpm =
                static_cast<int>(f.data[0]) |
                (static_cast<int>(f.data[1]) << 8) |
                (static_cast<int>(f.data[2]) << 16) |
                (static_cast<int>(f.data[3]) << 24);
            s.motor.measuredRpm = rpm;
            s.motor.temperatureC = f.data[4];
        }

        if (f.id == 0x183) {
            s.sensors.pressure.online = true;
            s.sensors.pressure.pressure =
                static_cast<double>(static_cast<int>(f.data[0]) | (static_cast<int>(f.data[1]) << 8));
            s.sensors.pressure.temperature = f.data[2];
        }

        if (f.id == 0x134) {
            s.sensors.navigation.accOnline = true;
            s.sensors.navigation.accX = f.data[0];
            s.sensors.navigation.accY = f.data[1];
            s.sensors.navigation.accZ = f.data[2];
        }

        if (f.id == 0x371) {
            s.sensors.temperatures[0].online = true;
            s.sensors.temperatures[0].temperatureC = f.data[0];
        }

        if (f.id >= 0x381 && f.id <= 0x384) {
            auto index = static_cast<std::size_t>(f.id - 0x381);
            if (index < s.gouvernes.axes.size()) {


           if (index < s.gouvernes.axes.size()) {
                s.gouvernes.axes[index].online = true;
                s.gouvernes.axes[index].operational = true;
                s.gouvernes.axes[index].positionDeg =
                    static_cast<double>(static_cast<int>(f.data[0]) | (static_cast<int>(f.data[1]) << 8));
            }
        }
    }
}


ifndef COMMUNICAN_SIMULATED_DEVICE_BACKEND_HPP
#define COMMUNICAN_SIMULATED_DEVICE_BACKEND_HPP

#include <cstdint>
#include "models/system_snapshot.hpp"
#include "sim/can_sim_bus.hpp"

class SimulatedDeviceBackend {
public:
    explicit SimulatedDeviceBackend(CanSimBus& bus);

    bool powerStartup();
    bool powerShutdown();

    bool configureMotor();
    bool startMotor();
    bool stopMotor();
    bool setMotorSpeed(std::int32_t rpm);

    bool configureGouvernes();
    bool startHomingGouvernes();
    bool setGouvernesOperational();
    bool moveGouvernesRaw(std::int32_t rawPosition);

    void poll(SystemSnapshot& snapshot);

private:
    void sendInt32(std::uint32_t id, std::int32_t value);

    CanSimBus& bus_;
    bool motorConfigured_ = false;
    bool motorEnabled_ = false;
    bool gouvConfigured_ = false;
    bool gouvOperational_ = false;
    PowerState power_{};
};

#endif





ifndef COMMUNICAN_CAN_SIM_FRAME_HPP
#define COMMUNICAN_CAN_SIM_FRAME_HPP

#include <array>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

struct CanSimFrame {
    std::uint32_t id = 0;
    std::uint8_t dlc = 8;
    std::array<std::uint8_t, 8> data{};

    std::string toString() const {
        std::ostringstream os;
        os << "ID=0x" << std::hex << std::uppercase << id
           << " DLC=" << std::dec << static_cast<int>(dlc) << " DATA=[";
        for (int i = 0; i < dlc; ++i) {
            if (i) os << " ";
            os << "0x" << std::hex << std::uppercase << std::setw(2)
               << std::setfill('0') << static_cast<int>(data[i]);
        }
        os << "]";
        return os.str();
    }
};

#endif





include "sim/can_sim_bus.hpp"
#include <chrono>
#include <iostream>

CanSimBus::~CanSimBus() {
    stop();
}

void CanSimBus::start() {
    if (running_) return;
    running_ = true;
    worker_ = std::thread(&CanSimBus::loop, this);
}

void CanSimBus::stop() {
    if (!running_) return;
    running_ = false;
    cv_.notify_all();
    if (worker_.joinable()) worker_.join();
}

void CanSimBus::send(const CanSimFrame& frame) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        txQueue_.push(frame);
    }
    cv_.notify_all();
}

bool CanSimBus::receive(CanSimFrame& outFrame) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (rxQueue_.empty()) return false;
    outFrame = rxQueue_.front();
    rxQueue_.pop();
    return true;
}

void CanSimBus::pushRx(const CanSimFrame& frame) {
    std::lock_guard<std::mutex> lock(mutex_);
    rxQueue_.push(frame);
}

void CanSimBus::loop() {
    using namespace std::chrono_literals;

    while (running_) {
        std::queue<CanSimFrame> localTx;

        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait_for(lock, 50ms, [&] { return !txQueue_.empty() || !running_; });
            while (!txQueue_.empty()) {
                localTx.push(txQueue_.front());
                txQueue_.pop();
            }
        }

        while (!localTx.empty()) {
            auto frame = localTx.front();
            localTx.pop();

            std::cout << "[CAN_SIM][TX] " << frame.toString() << "\n";

            if (frame.id == 0x201 && frame.dlc >= 4) {
                targetMotorRpm_ =
                    static_cast<int>(frame.data[0]) |
                    (static_cast<int>(frame.data[1]) << 8) |
                    (static_cast<int>(frame.data[2]) << 16) |
                    (static_cast<int>(frame.data[3]) << 24);

                CanSimFrame ack;
                ack.id = 0x181;
                ack.dlc = 8;
                ack.data[4] = 42;
                pushRx(ack);
            }

            if (frame.id >= 0x401 && frame.id <= 0x404 && frame.dlc >= 4) {
                CanSimFrame gouv;
                gouv.id = 0x381 + (frame.id - 0x401);
                gouv.dlc = 8;
                gouv.data = frame.data;
                gouv.data[4] = 1;
                pushRx(gouv);
            }
        }

        if (motorRpm_ < targetMotorRpm_) motorRpm_ += 50;
        if (motorRpm_ > targetMotorRpm_) motorRpm_ -= 50;

        tick_++;

        if (tick_ % 4 == 0) {
            CanSimFrame motor;
            motor.id = 0x181;
            motor.dlc = 8;
            motor.data[0] = motorRpm_ & 0xFF;
            motor.data[1] = (motorRpm_ >> 8) & 0xFF;
            motor.data[2] = (motorRpm_ >> 16) & 0xFF;
            motor.data[3] = (motorRpm_ >> 24) & 0xFF;
            motor.data[4] = 42;
            pushRx(motor);
        }

        if (tick_ % 6 == 0) {
            CanSimFrame pressure;
            pressure.id = 0x183;
            pressure.dlc = 8;
            pressure.data[0] = 100;
            pressure.data[1] = 0;
            pressure.data[2] = 25;
            pushRx(pressure);
        }

        if (tick_ % 8 == 0) {
            CanSimFrame nav;
            nav.id = 0x134;
            nav.dlc = 8;
            nav.data[0] = 1;
            nav.data[1] = 2;
            nav.data[2] = 3;
            pushRx(nav);
        }

        if (tick_ % 10 == 0) {
            CanSimFrame temp;
            temp.id = 0x371;
            temp.dlc = 8;
            temp.data[0] = 30;
            pushRx(temp);
        }
    }
}















