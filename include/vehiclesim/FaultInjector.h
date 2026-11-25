#ifndef VEHICLESIM_FAULTINJECTOR_H
#define VEHICLESIM_FAULTINJECTOR_H

#include "CoreTypes.h"

#include <string>
#include <vector>

namespace vehiclesim {

class FaultInjector {
public:
    explicit FaultInjector(std::vector<Fault> faults);

    void setEnabled(bool enabled);

    FaultEffect effectFor(const std::string& sensorId, SimulationTime nowMs) const;
    std::vector<std::string> activeLabelsFor(const std::string& sensorId,
                                              SimulationTime nowMs) const;

    bool hasFaults() const;

private:
    bool isActive(const Fault& fault, const std::string& sensorId,
                  SimulationTime nowMs) const;

    std::vector<Fault> faults_;
    bool enabled_;
};

} // namespace vehiclesim

#endif // VEHICLESIM_FAULTINJECTOR_H
