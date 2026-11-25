#include "DiagnosticEngine.h"

#include <sstream>

namespace vehiclesim {

Diagnosis DiagnosticEngine::evaluate(const SensorDefinition& definition,
                                     const SensorReading& reading) const {
    Diagnosis diagnosis;
    std::ostringstream message;

    if (reading.value <= definition.criticalLow) {
        diagnosis.severity = Severity::Critical;
        message << definition.label << " critically low: " << reading.value << " "
                << reading.unit;
    } else if (reading.value >= definition.criticalHigh) {
        diagnosis.severity = Severity::Critical;
        message << definition.label << " critically high: " << reading.value << " "
                << reading.unit;
    } else if (reading.value <= definition.warningLow) {
        diagnosis.severity = Severity::Warning;
        message << definition.label << " low: " << reading.value << " " << reading.unit;
    } else if (reading.value >= definition.warningHigh) {
        diagnosis.severity = Severity::Warning;
        message << definition.label << " high: " << reading.value << " " << reading.unit;
    } else {
        diagnosis.severity = Severity::Ok;
        message << definition.label << " normal: " << reading.value << " "
                << reading.unit;
    }

    diagnosis.message = message.str();
    return diagnosis;
}

} // namespace vehiclesim
