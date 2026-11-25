#ifndef VEHICLESIM_SENSORDEFINITION_H
#define VEHICLESIM_SENSORDEFINITION_H

#include "CoreTypes.h"

#include <string>

namespace vehiclesim {

enum class SensorModel { Generic, Thermal, Vibration };

struct SensorDefinition {
    std::string id;
    std::string label;
    std::string unit;

    double initialValue = 0.0;
    double minValue = 0.0;
    double maxValue = 1.0;

    double nominalLow = 0.0;
    double nominalHigh = 1.0;

    double warningLow = 0.0;
    double warningHigh = 1.0;

    double criticalLow = 0.0;
    double criticalHigh = 1.0;

    Milliseconds samplePeriod = Milliseconds(1000);
    double volatility = 0.0;
    double driftRate = 0.01;
    double nominalTarget = 0.5;

    SensorModel model = SensorModel::Generic;
    double modelParam1 = 0.0;
    double modelParam2 = 0.0;
};

void validateSensorDefinition(const SensorDefinition& definition);

class SensorDefinitionBuilder {
public:
    SensorDefinitionBuilder(std::string id, std::string label, std::string unit,
                            double initialValue);

    SensorDefinitionBuilder& range(double minimum, double maximum);
    SensorDefinitionBuilder& nominal(double low, double high);
    SensorDefinitionBuilder& warning(double low, double high);
    SensorDefinitionBuilder& critical(double low, double high);
    SensorDefinitionBuilder& sampleEvery(Milliseconds period);
    SensorDefinitionBuilder& noise(double volatility);
    SensorDefinitionBuilder& drift(double rate, double target);
    SensorDefinitionBuilder& model(SensorModel model);
    SensorDefinitionBuilder& modelParams(double first, double second);

    SensorDefinition build() const;

private:
    SensorDefinition definition_;
};

} // namespace vehiclesim

#endif // VEHICLESIM_SENSORDEFINITION_H
