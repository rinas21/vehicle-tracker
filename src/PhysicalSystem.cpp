#include "PhysicalSystem.h"

#include "CoreTypes.h"
#include "sensors/SimulatedSensor.h"
#include "sensors/ThermalSensor.h"
#include "sensors/VibrationSensor.h"

#include <algorithm>
#include <utility>

namespace vehiclesim {

std::unique_ptr<ISensor> PhysicalSystem::createSensor(const std::string& systemName,
                                                      const SensorDefinition& definition,
                                                      std::uint32_t seed) {
    switch (definition.model) {
        case SensorModel::Thermal:
            return std::unique_ptr<ISensor>(new ThermalSensor(
                systemName, definition, seed,
                definition.modelParam1 > 0.0 ? definition.modelParam1 : 3.0));
        case SensorModel::Vibration:
            return std::unique_ptr<ISensor>(new VibrationSensor(
                systemName, definition, seed,
                definition.modelParam1, definition.modelParam2));
        case SensorModel::Generic:
        default:
            return std::unique_ptr<ISensor>(
                new SimulatedSensor(systemName, definition, seed));
    }
}

PhysicalSystem::PhysicalSystem(SystemProfile profile) : profile_(std::move(profile)) {
    for (std::size_t index = 0; index < profile_.sensors.size(); ++index) {
        const SensorDefinition& definition = profile_.sensors[index];
        validateSensorDefinition(definition);

        const std::uint32_t salt = static_cast<std::uint32_t>(index) * 0x9E3779B9u;
        const std::uint32_t seed = profile_.baseSeed ^ fnv1a32(definition.id) ^ salt;

        sensors_.push_back(createSensor(profile_.name, definition, seed));
    }
}

const SystemProfile& PhysicalSystem::profile() const {
    return profile_;
}

std::vector<std::unique_ptr<ISensor>>& PhysicalSystem::sensors() {
    return sensors_;
}

const std::vector<std::unique_ptr<ISensor>>& PhysicalSystem::sensors() const {
    return sensors_;
}

} // namespace vehiclesim
