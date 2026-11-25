#include <gtest/gtest.h>

#include "FaultInjector.h"

#include <vector>

using namespace vehiclesim;

namespace {

Fault makeOffsetFault(const std::string& sensorId, double magnitude,
                      SimulationTime startMs, SimulationTime durationMs) {
    Fault fault;
    fault.sensorId = sensorId;
    fault.mode = FaultMode::Offset;
    fault.magnitude = magnitude;
    fault.startMs = startMs;
    fault.durationMs = durationMs;
    fault.label = sensorId + "_fault";
    return fault;
}

} // namespace

TEST(FaultTests, OffsetFaultIsActiveOnlyInsideItsWindow) {
    FaultInjector injector({makeOffsetFault("engine_temp", 25.0, 1000, 1000)});

    EXPECT_DOUBLE_EQ(injector.effectFor("engine_temp", 500).offset, 0.0);
    EXPECT_DOUBLE_EQ(injector.effectFor("engine_temp", 1500).offset, 25.0);
    EXPECT_DOUBLE_EQ(injector.effectFor("engine_temp", 2500).offset, 0.0);
}

TEST(FaultTests, ScaleFaultMultipliesReportedValue) {
    Fault scaleFault;
    scaleFault.sensorId = "current";
    scaleFault.mode = FaultMode::Scale;
    scaleFault.magnitude = 1.5;
    scaleFault.startMs = 0;
    scaleFault.durationMs = 5000;
    scaleFault.label = "overcurrent";

    FaultInjector injector({scaleFault});
    EXPECT_DOUBLE_EQ(injector.effectFor("current", 1000).scale, 1.5);
}

TEST(FaultTests, DisabledInjectorProducesNoEffect) {
    FaultInjector injector({makeOffsetFault("engine_temp", 25.0, 0, 5000)});
    injector.setEnabled(false);

    const FaultEffect effect = injector.effectFor("engine_temp", 1000);
    EXPECT_DOUBLE_EQ(effect.offset, 0.0);
    EXPECT_DOUBLE_EQ(effect.scale, 1.0);
    EXPECT_TRUE(injector.activeLabelsFor("engine_temp", 1000).empty());
}

TEST(FaultTests, OverlappingOffsetFaultsAccumulate) {
    std::vector<Fault> faults;
    faults.push_back(makeOffsetFault("temp", 10.0, 0, 5000));
    faults.push_back(makeOffsetFault("temp", 5.0, 0, 5000));

    FaultInjector injector(faults);
    EXPECT_DOUBLE_EQ(injector.effectFor("temp", 1000).offset, 15.0);
    EXPECT_EQ(injector.activeLabelsFor("temp", 1000).size(), 2u);
}
