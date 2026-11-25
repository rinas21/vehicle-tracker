#ifndef VEHICLESIM_PROFILEREGISTRY_H
#define VEHICLESIM_PROFILEREGISTRY_H

#include "SystemProfile.h"

#include <string>
#include <vector>

namespace vehiclesim {

SystemProfile makeProfile(const std::string& name);
std::vector<std::string> availableProfileNames();

} // namespace vehiclesim

#endif // VEHICLESIM_PROFILEREGISTRY_H
