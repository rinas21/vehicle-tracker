#ifndef VEHICLESIM_PHYSICALSYSTEM_H
#define VEHICLESIM_PHYSICALSYSTEM_H

#include "ISensor.h"
#include "SystemProfile.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace vehiclesim {

class PhysicalSystem {
public:
    explicit PhysicalSystem(SystemProfile profile);

    const SystemProfile& profile() const;

    std::vector<std::unique_ptr<ISensor>>& sensors();
    const std::vector<std::unique_ptr<ISensor>>& sensors() const;

private:
    SystemProfile profile_;
    std::vector<std::unique_ptr<ISensor>> sensors_;

    static std::unique_ptr<ISensor> createSensor(const std::string& systemName,
                                                 const SensorDefinition& definition,
                                                 std::uint32_t seed);
};

} // namespace vehiclesim

#endif // VEHICLESIM_PHYSICALSYSTEM_H
