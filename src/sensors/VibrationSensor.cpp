#include "sensors/VibrationSensor.h"

#include <algorithm>
#include <utility>

namespace vehiclesim {

VibrationSensor::VibrationSensor(std::string systemName, const SensorDefinition& definition,
                                 std::uint32_t seed, double spikeProbability,
                                 double spikeMagnitude)
    : SimulatedSensor(std::move(systemName), definition, seed),
      spikeDistribution_(std::max(0.0, std::min(1.0, spikeProbability))),
      spikeScaleDistribution_(0.5, 1.0),
      spikeMagnitude_(spikeMagnitude) {}

double VibrationSensor::advancePhysicalValue(Milliseconds deltaTime) {
    double value = SimulatedSensor::advancePhysicalValue(deltaTime);
    if (spikeDistribution_(rng())) {
        value += spikeMagnitude_ * spikeScaleDistribution_(rng());
    }
    return clampToRange(value);
}

} // namespace vehiclesim
