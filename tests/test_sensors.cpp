#include <gtest/gtest.h>

#include "PhysicalSystem.h"
#include "SensorDefinition.h"
#include "sensors/SimulatedSensor.h"
#include "sensors/ThermalSensor.h"
#include "sensors/VibrationSensor.h"
#include "profiles/VehicleProfile.h"

using namespace vehiclesim;

namespace {

SensorDefinition makeDefinition() {
    return SensorDefinitionBuilder("test_temp", "Test Temperature", "C", 20.0)
        .range(0.0, 200.0)
        .nominal(80.0, 120.0)
        .warning(60.0, 140.0)
        .critical(40.0, 160.0)
        .sampleEvery(Milliseconds(500))
        .noise(0.1)
        .drift(0.5, 100.0)
        .build();
}

} // namespace

TEST(SensorTests, SameSeedProducesSameSequence) {
    const SensorDefinition definition = makeDefinition();

    SimulatedSensor first("test", definition, 1234u);
    SimulatedSensor second("test", definition, 1234u);

    const FaultEffect noFault;
    for (int index = 0; index < 8; ++index) {
        const SimulationTime now = index * 500;
        EXPECT_DOUBLE_EQ(first.sample(now, Milliseconds(500), noFault).value,
                         second.sample(now, Milliseconds(500), noFault).value);
    }
}

TEST(SensorTests, DifferentSeedsProduceDifferentSequences) {
    const SensorDefinition definition = makeDefinition();

    SimulatedSensor first("test", definition, 1234u);
    SimulatedSensor second("test", definition, 5678u);

    const FaultEffect noFault;
    bool foundDifference = false;
    for (int index = 0; index < 12; ++index) {
        const SimulationTime now = index * 500;
        if (first.sample(now, Milliseconds(500), noFault).value !=
            second.sample(now, Milliseconds(500), noFault).value) {
            foundDifference = true;
            break;
        }
    }
    EXPECT_TRUE(foundDifference);
}

TEST(SensorTests, FaultOffsetIsClampedToPhysicalRange) {
    const SensorDefinition definition = makeDefinition();
    SimulatedSensor sensor("test", definition, 42u);

    FaultEffect extremeFault;
    extremeFault.offset = 1000.0;
    const SensorReading reading =
        sensor.sample(0, Milliseconds(500), extremeFault);

    EXPECT_LE(reading.value, definition.maxValue);
    EXPECT_GE(reading.value, definition.minValue);
}

TEST(SensorTests, ThermalSensorMovesTowardTarget) {
    SensorDefinition definition = makeDefinition();
    definition.initialValue = 20.0;
    definition.nominalTarget = 100.0;
    definition.volatility = 0.0;

    ThermalSensor sensor("test", definition, 7u, 0.1);
    const SensorReading reading =
        sensor.sample(1000, Milliseconds(1000), FaultEffect());

    EXPECT_GT(reading.value, 20.0);
    EXPECT_LE(reading.value, 100.0);
}

TEST(SensorTests, VibrationSensorAddsGuaranteedSpike) {
    SensorDefinition definition = makeDefinition();
    definition.initialValue = 10.0;
    definition.nominalTarget = 10.0;
    definition.volatility = 0.0;

    VibrationSensor sensor("test", definition, 9u, 1.0, 20.0);
    const SensorReading reading =
        sensor.sample(500, Milliseconds(500), FaultEffect());

    EXPECT_GE(reading.value, 15.0);
}

TEST(SensorTests, VehicleProfileBuildsFourIndependentSensors) {
    const SystemProfile profile = makeVehicleProfile();
    const PhysicalSystem system(profile);

    ASSERT_EQ(profile.sensors.size(), 4u);
    ASSERT_EQ(system.sensors().size(), 4u);

    EXPECT_EQ(system.sensors()[0]->id(), "engine_temp");
    EXPECT_EQ(system.sensors()[3]->id(), "oil_pressure");
}
