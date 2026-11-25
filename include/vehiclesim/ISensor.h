#ifndef VEHICLESIM_ISENSOR_H
#define VEHICLESIM_ISENSOR_H

#include "CoreTypes.h"

#include <string>

namespace vehiclesim {

class ISensor {
public:
    virtual ~ISensor() {}

    virtual const std::string& id() const = 0;
    virtual const std::string& label() const = 0;
    virtual const std::string& unit() const = 0;

    virtual SensorReading sample(SimulationTime timestampMs,
                                 Milliseconds deltaTime,
                                 const FaultEffect& faultEffect) = 0;
};

} // namespace vehiclesim

#endif // VEHICLESIM_ISENSOR_H
