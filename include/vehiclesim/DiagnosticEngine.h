#ifndef VEHICLESIM_DIAGNOSTICENGINE_H
#define VEHICLESIM_DIAGNOSTICENGINE_H

#include "CoreTypes.h"
#include "SensorDefinition.h"

namespace vehiclesim {

class DiagnosticEngine {
public:
    Diagnosis evaluate(const SensorDefinition& definition,
                       const SensorReading& reading) const;
};

} // namespace vehiclesim

#endif // VEHICLESIM_DIAGNOSTICENGINE_H
