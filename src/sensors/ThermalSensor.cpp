#include "sensors/ThermalSensor.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace vehiclesim {

ThermalSensor::ThermalSensor(std::string systemName, const SensorDefinition& definition,
                             std::uint32_t seed, double timeConstantSeconds)
    : SimulatedSensor(std::move(systemName), definition, seed),
      timeConstantSeconds_(std::max(0.05, timeConstantSeconds)) {}

double ThermalSensor::advancePhysicalValue(Milliseconds deltaTime) {
    const double seconds = deltaTime.count() > 0
                               ? static_cast<double>(deltaTime.count()) / 1000.0
                               : 0.001;
    const double response = 1.0 - std::exp(-seconds / timeConstantSeconds_);
    const double target = definition().nominalTarget;
    const double current = currentValue();

    const double next = current + response * (target - current) + randomNoise(deltaTime);
    return clampToRange(next);
}

} // namespace vehiclesim
