#include "profiles/ProfileRegistry.h"

#include "profiles/MotorProfile.h"
#include "profiles/PumpProfile.h"
#include "profiles/VehicleProfile.h"

#include <stdexcept>

namespace vehiclesim {

SystemProfile makeProfile(const std::string& name) {
    if (name == "vehicle") {
        return makeVehicleProfile();
    }
    if (name == "motor" || name == "industrial_motor") {
        return makeMotorProfile();
    }
    if (name == "pump" || name == "water_pump") {
        return makePumpProfile();
    }
    throw std::invalid_argument("Unknown profile: " + name);
}

std::vector<std::string> availableProfileNames() {
    return {"vehicle", "industrial_motor", "water_pump"};
}

} // namespace vehiclesim
