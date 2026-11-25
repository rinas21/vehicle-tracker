#ifndef VEHICLESIM_VIBRATIONSENSOR_H
#define VEHICLESIM_VIBRATIONSENSOR_H

#include "sensors/SimulatedSensor.h"

#include <random>

namespace vehiclesim {

class VibrationSensor : public SimulatedSensor {
public:
    VibrationSensor(std::string systemName, const SensorDefinition& definition,
                    std::uint32_t seed, double spikeProbability, double spikeMagnitude);

protected:
    double advancePhysicalValue(Milliseconds deltaTime) override;

private:
    std::bernoulli_distribution spikeDistribution_;
    std::uniform_real_distribution<double> spikeScaleDistribution_;
    double spikeMagnitude_;
};

} // namespace vehiclesim

#endif // VEHICLESIM_VIBRATIONSENSOR_H
