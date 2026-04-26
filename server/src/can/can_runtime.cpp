#include "can/can_runtime.hpp"

#include "config/runtime_mode.hpp"
#include "sim/state_simulator.hpp"

void start_can_runtime() {
if (g_runtime_mode == RuntimeMode::SIMULATION) {
start_state_simulator();
}
}