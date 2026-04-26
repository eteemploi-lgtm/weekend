#include "models/global_state.hpp"

// Version minimale cohérente.
// À remplacer plus tard par le vrai décodage CAN.

class CanManager {
public:
void poll_once() {
std::lock_guard<std::mutex> lock(g_state_mutex);

// Placeholder : rien à décoder ici pour l’instant.
// La simulation alimente déjà g_state.
g_state.system.server_status = "online";
}
};