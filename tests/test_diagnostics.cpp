#include <gtest/gtest.h>

#include "DiagnosticEngine.h"
#include "SensorDefinition.h"
#include "profiles/VehicleProfile.h"

#include <stdexcept>

using namespace vehiclesim;

namespace {

SensorDefinition makeDefinition() {
    return SensorDefinitionBuilder("generic", "Generic Sensor", "unit", 50.0)
        .range(0.0, 100.0)
        .nominal(30.0, 70.0)
        .warning(20.0, 80.0)
        .critical(10.0, 90.0)
        .sampleEvery(Milliseconds(500))
        .build();
}

SensorReading makeReading(double value) {
    SensorReading reading;
    reading.value = value;
    return reading;
}

} // namespace

TEST(DiagnosticTests, ClassifiesAllSeverityBands) {
    const DiagnosticEngine engine;
    const SensorDefinition definition = makeDefinition();

    EXPECT_EQ(engine.evaluate(definition, makeReading(5.0)).severity, Severity::Critical);
    EXPECT_EQ(engine.evaluate(definition, makeReading(15.0)).severity, Severity::Warning);
    EXPECT_EQ(engine.evaluate(definition, makeReading(50.0)).severity, Severity::Ok);
    EXPECT_EQ(engine.evaluate(definition, makeReading(85.0)).severity, Severity::Warning);
    EXPECT_EQ(engine.evaluate(definition, makeReading(95.0)).severity, Severity::Critical);
}

TEST(DiagnosticTests, DiagnosisMessageIsDescriptive) {
    const DiagnosticEngine engine;
    const Diagnosis diagnosis = engine.evaluate(makeDefinition(), makeReading(95.0));

    EXPECT_NE(diagnosis.message.find("critically high"), std::string::npos);
    EXPECT_NE(diagnosis.message.find("95"), std::string::npos);
}

TEST(DiagnosticTests, RejectsInvalidThresholdOrdering) {
    SensorDefinition invalid = makeDefinition();
    invalid.warningLow = 35.0; // above nominalLow

    EXPECT_THROW(validateSensorDefinition(invalid), std::invalid_argument);
}

TEST(DiagnosticTests, AllProfileDefinitionsAreValid) {
    const SystemProfile profile = makeVehicleProfile();
    for (const SensorDefinition& definition : profile.sensors) {
        EXPECT_NO_THROW(validateSensorDefinition(definition));
    }
}
