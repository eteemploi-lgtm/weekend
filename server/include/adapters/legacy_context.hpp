#ifndef COMMUNICAN_LEGACY_CONTEXT_HPP
#define COMMUNICAN_LEGACY_CONTEXT_HPP

#include <mutex>
#include <memory>
#include <cstdint>

extern "C" {
#include "dataCommon.h"
}

/**
 * LegacyContext
 * -------------
 * Owns the legacy C shared context used by the old CAN/CANopen code.
 *
 * Rule:
 * - Legacy C code may still use stDataCommon.
 * - Modern C++ code must access it through this context or through adapters.
 * - Do not expose stDataCommon directly to API routes or WebSocket code.
 */
class LegacyContext {
public:
    LegacyContext();
    ~LegacyContext() = default;

    LegacyContext(const LegacyContext&) = delete;
    LegacyContext& operator=(const LegacyContext&) = delete;

    stDataCommon* raw();
    const stDataCommon* raw() const;

    std::mutex& mutex();

private:
    stDataCommon data_{};
    std::mutex mutex_;
};

#endif
