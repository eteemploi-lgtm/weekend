#pragma once

class CanManager {
public:
    bool init();
    void poll_once();

    bool send_motor_rpm_cmd(int rpm);
    bool send_gouverne_position_cmd(double position);
};