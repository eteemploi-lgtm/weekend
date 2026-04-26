#include "adapters/legacy_context.hpp"

#include <cstring>

LegacyContext::LegacyContext() {
    std::memset(&data_, 0, sizeof(data_));
    /*
     * If your legacy project has a real init function for stDataCommon,
     * call it here instead of using memset.
     *
     * Example:
     *   InitDataCommon(&data_);
     */
}

stDataCommon* LegacyContext::raw() {
    return &data_;
}

const stDataCommon* LegacyContext::raw() const {
    return &data_;
}

std::mutex& LegacyContext::mutex() {
    return mutex_;
}
