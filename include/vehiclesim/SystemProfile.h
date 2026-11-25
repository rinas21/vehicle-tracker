#ifndef VEHICLESIM_SYSTEMPROFILE_H
#define VEHICLESIM_SYSTEMPROFILE_H

#include "CoreTypes.h"
#include "SensorDefinition.h"

#include <cstdint>
#include <string>
#include <vector>

namespace vehiclesim {

struct SystemProfile {
    std::string name;
    std::uint32_t baseSeed = 0u;
    std::vector<SensorDefinition> sensors;
    std::vector<Fault> faults;
};

} // namespace vehiclesim

#endif // VEHICLESIM_SYSTEMPROFILE_H
