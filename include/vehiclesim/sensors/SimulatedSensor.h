#ifndef VEHICLESIM_SIMULATEDSENSOR_H
#define VEHICLESIM_SIMULATEDSENSOR_H

#include "ISensor.h"
#include "SensorDefinition.h"

#include <cstdint>
#include <random>
#include <string>

namespace vehiclesim {

class SimulatedSensor : public ISensor {
public:
    SimulatedSensor(std::string systemName, const SensorDefinition& definition,
                    std::uint32_t seed);
    ~SimulatedSensor() override;

    const std::string& id() const override;
    const std::string& label() const override;
    const std::string& unit() const override;

    SensorReading sample(SimulationTime timestampMs, Milliseconds deltaTime,
                         const FaultEffect& faultEffect) override;

protected:
    virtual double advancePhysicalValue(Milliseconds deltaTime);
    double randomNoise(Milliseconds deltaTime);
    double clampToRange(double value) const;

    double currentValue() const;
    std::mt19937& rng();
    const SensorDefinition& definition() const;

private:
    std::string systemName_;
    SensorDefinition definition_;
    std::mt19937 rng_;
    std::uniform_real_distribution<double> noiseDistribution_;
    double value_;
};

} // namespace vehiclesim

#endif // VEHICLESIM_SIMULATEDSENSOR_H
