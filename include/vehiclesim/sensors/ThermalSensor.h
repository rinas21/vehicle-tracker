#ifndef VEHICLESIM_THERMALSENSOR_H
#define VEHICLESIM_THERMALSENSOR_H

#include "sensors/SimulatedSensor.h"

namespace vehiclesim {

class ThermalSensor : public SimulatedSensor {
public:
    ThermalSensor(std::string systemName, const SensorDefinition& definition,
                  std::uint32_t seed, double timeConstantSeconds);

protected:
    double advancePhysicalValue(Milliseconds deltaTime) override;

private:
    double timeConstantSeconds_;
};

} // namespace vehiclesim

#endif // VEHICLESIM_THERMALSENSOR_H
