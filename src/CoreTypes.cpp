#include "CoreTypes.h"

namespace vehiclesim {

const char* toString(Severity severity) {
    switch (severity) {
        case Severity::Ok:
            return "OK";
        case Severity::Warning:
            return "WARNING";
        case Severity::Critical:
            return "CRITICAL";
    }
    return "UNKNOWN";
}

std::uint32_t fnv1a32(const std::string& text) {
    std::uint32_t hash = 2166136261u;
    for (unsigned char character : text) {
        hash ^= static_cast<std::uint32_t>(character);
        hash *= 16777619u;
    }
    return hash;
}

} // namespace vehiclesim
