#ifndef VEHICLESIM_CORETYPES_H
#define VEHICLESIM_CORETYPES_H

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace vehiclesim {

using Milliseconds = std::chrono::milliseconds;
using SimulationTime = std::int64_t;

enum class Severity { Ok, Warning, Critical };

enum class FaultMode { Offset, Scale };

struct Fault {
    std::string sensorId;
    FaultMode mode = FaultMode::Offset;
    double magnitude = 0.0;
    SimulationTime startMs = 0;
    SimulationTime durationMs = 0;
    std::string label;
};

struct FaultEffect {
    double offset = 0.0;
    double scale = 1.0;
};

struct SensorReading {
    SimulationTime timestampMs = 0;
    std::string systemName;
    std::string sensorId;
    std::string sensorLabel;
    std::string unit;
    double value = 0.0;
    Severity severity = Severity::Ok;
    std::vector<std::string> activeFaults;
};

struct Diagnosis {
    Severity severity = Severity::Ok;
    std::string message;
};

const char* toString(Severity severity);
std::uint32_t fnv1a32(const std::string& text);

} // namespace vehiclesim

#endif // VEHICLESIM_CORETYPES_H
