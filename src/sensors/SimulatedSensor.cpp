#include "sensors/SimulatedSensor.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace vehiclesim {

SimulatedSensor::SimulatedSensor(std::string systemName, const SensorDefinition& definition,
                                 std::uint32_t seed)
    : systemName_(std::move(systemName)),
      definition_(definition),
      rng_(seed),
      noiseDistribution_(-1.0, 1.0),
      value_(definition.initialValue) {}

SimulatedSensor::~SimulatedSensor() = default;

const std::string& SimulatedSensor::id() const {
    return definition_.id;
}

const std::string& SimulatedSensor::label() const {
    return definition_.label;
}

const std::string& SimulatedSensor::unit() const {
    return definition_.unit;
}

const SensorDefinition& SimulatedSensor::definition() const {
    return definition_;
}

double SimulatedSensor::currentValue() const {
    return value_;
}

std::mt19937& SimulatedSensor::rng() {
    return rng_;
}

double SimulatedSensor::clampToRange(double value) const {
    return std::max(definition_.minValue, std::min(definition_.maxValue, value));
}

double SimulatedSensor::randomNoise(Milliseconds deltaTime) {
    const double seconds = deltaTime.count() > 0
                               ? static_cast<double>(deltaTime.count()) / 1000.0
                               : 0.001;
    const double amplitude = definition_.volatility * std::sqrt(seconds);
    return noiseDistribution_(rng_) * amplitude;
}

double SimulatedSensor::advancePhysicalValue(Milliseconds deltaTime) {
    const double seconds = deltaTime.count() > 0
                               ? static_cast<double>(deltaTime.count()) / 1000.0
                               : 0.001;
    const double pull = definition_.driftRate * seconds *
                        (definition_.nominalTarget - value_);
    return clampToRange(value_ + pull + randomNoise(deltaTime));
}

SensorReading SimulatedSensor::sample(SimulationTime timestampMs, Milliseconds deltaTime,
                                      const FaultEffect& faultEffect) {
    SensorReading reading;
    reading.timestampMs = timestampMs;
    reading.systemName = systemName_;
    reading.sensorId = definition_.id;
    reading.sensorLabel = definition_.label;
    reading.unit = definition_.unit;
    reading.severity = Severity::Ok;

    value_ = clampToRange(advancePhysicalValue(deltaTime));

    const double reportedValue = (value_ + faultEffect.offset) * faultEffect.scale;
    reading.value = clampToRange(reportedValue);
    return reading;
}

} // namespace vehiclesim
