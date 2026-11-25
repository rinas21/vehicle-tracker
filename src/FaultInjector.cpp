#include "FaultInjector.h"

#include <utility>

namespace vehiclesim {

FaultInjector::FaultInjector(std::vector<Fault> faults)
    : faults_(std::move(faults)), enabled_(true) {}

void FaultInjector::setEnabled(bool enabled) {
    enabled_ = enabled;
}

bool FaultInjector::hasFaults() const {
    return !faults_.empty();
}

bool FaultInjector::isActive(const Fault& fault, const std::string& sensorId,
                             SimulationTime nowMs) const {
    if (!enabled_ || fault.sensorId != sensorId || fault.durationMs <= 0) {
        return false;
    }
    return nowMs >= fault.startMs && nowMs < fault.startMs + fault.durationMs;
}

FaultEffect FaultInjector::effectFor(const std::string& sensorId,
                                     SimulationTime nowMs) const {
    FaultEffect effect;

    for (const Fault& fault : faults_) {
        if (!isActive(fault, sensorId, nowMs)) {
            continue;
        }

        if (fault.mode == FaultMode::Offset) {
            effect.offset += fault.magnitude;
        } else {
            effect.scale *= fault.magnitude;
        }
    }

    return effect;
}

std::vector<std::string> FaultInjector::activeLabelsFor(const std::string& sensorId,
                                                        SimulationTime nowMs) const {
    std::vector<std::string> labels;

    for (const Fault& fault : faults_) {
        if (isActive(fault, sensorId, nowMs) && !fault.label.empty()) {
            labels.push_back(fault.label);
        }
    }

    return labels;
}

} // namespace vehiclesim
