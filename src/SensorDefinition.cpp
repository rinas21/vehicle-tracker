#include "SensorDefinition.h"

#include <stdexcept>
#include <utility>

namespace vehiclesim {

void validateSensorDefinition(const SensorDefinition& definition) {
    if (definition.id.empty()) {
        throw std::invalid_argument("Sensor id must not be empty");
    }
    if (definition.label.empty()) {
        throw std::invalid_argument("Sensor label must not be empty: " + definition.id);
    }
    if (!(definition.minValue < definition.maxValue)) {
        throw std::invalid_argument("Sensor min must be less than max: " + definition.id);
    }
    if (!(definition.criticalLow <= definition.warningLow &&
          definition.warningLow <= definition.nominalLow &&
          definition.nominalLow <= definition.nominalHigh &&
          definition.nominalHigh <= definition.warningHigh &&
          definition.warningHigh <= definition.criticalHigh)) {
        throw std::invalid_argument("Sensor threshold ordering invalid: " + definition.id);
    }
    if (definition.samplePeriod.count() <= 0) {
        throw std::invalid_argument("Sensor sample period must be positive: " + definition.id);
    }
}

SensorDefinitionBuilder::SensorDefinitionBuilder(std::string id, std::string label,
                                                 std::string unit, double initialValue) {
    definition_.id = std::move(id);
    definition_.label = std::move(label);
    definition_.unit = std::move(unit);
    definition_.initialValue = initialValue;
    definition_.nominalTarget = initialValue;
}

SensorDefinitionBuilder& SensorDefinitionBuilder::range(double minimum, double maximum) {
    definition_.minValue = minimum;
    definition_.maxValue = maximum;
    return *this;
}

SensorDefinitionBuilder& SensorDefinitionBuilder::nominal(double low, double high) {
    definition_.nominalLow = low;
    definition_.nominalHigh = high;
    definition_.nominalTarget = (low + high) / 2.0;
    return *this;
}

SensorDefinitionBuilder& SensorDefinitionBuilder::warning(double low, double high) {
    definition_.warningLow = low;
    definition_.warningHigh = high;
    return *this;
}

SensorDefinitionBuilder& SensorDefinitionBuilder::critical(double low, double high) {
    definition_.criticalLow = low;
    definition_.criticalHigh = high;
    return *this;
}

SensorDefinitionBuilder& SensorDefinitionBuilder::sampleEvery(Milliseconds period) {
    definition_.samplePeriod = period;
    return *this;
}

SensorDefinitionBuilder& SensorDefinitionBuilder::noise(double volatility) {
    definition_.volatility = volatility;
    return *this;
}

SensorDefinitionBuilder& SensorDefinitionBuilder::drift(double rate, double target) {
    definition_.driftRate = rate;
    definition_.nominalTarget = target;
    return *this;
}

SensorDefinitionBuilder& SensorDefinitionBuilder::model(SensorModel sensorModel) {
    definition_.model = sensorModel;
    return *this;
}

SensorDefinitionBuilder& SensorDefinitionBuilder::modelParams(double first, double second) {
    definition_.modelParam1 = first;
    definition_.modelParam2 = second;
    return *this;
}

SensorDefinition SensorDefinitionBuilder::build() const {
    validateSensorDefinition(definition_);
    return definition_;
}

} // namespace vehiclesim
