#ifndef COMMUNICAN_JSON_SERIALIZATION_HPP
#define COMMUNICAN_JSON_SERIALIZATION_HPP

#include <string>
#include "models/system_snapshot.hpp"

/**
 * Minimal JSON serialization without external dependency.
 * Replace by nlohmann::json later if your project already uses it.
 */
std::string toJson(const SystemSnapshot& snapshot);

#endif
