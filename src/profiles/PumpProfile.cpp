#include "profiles/PumpProfile.h"

#include "SensorDefinition.h"

namespace vehiclesim {

SystemProfile makePumpProfile() {
    SystemProfile profile;
    profile.name = "water_pump";
    profile.baseSeed = 0x000A11CEu;

    profile.sensors.push_back(
        SensorDefinitionBuilder("water_pressure", "Water Pressure", "bar", 4.6)
            .range(0.0, 12.0)
            .nominal(3.5, 5.5)
            .warning(3.0, 6.5)
            .critical(2.0, 8.0)
            .sampleEvery(Milliseconds(500))
            .noise(0.12)
            .drift(0.30, 4.6)
            .build());

    profile.sensors.push_back(
        SensorDefinitionBuilder("flow_rate", "Flow Rate", "L/min", 70.0)
            .range(0.0, 120.0)
            .nominal(55.0, 85.0)
            .warning(45.0, 95.0)
            .critical(30.0, 110.0)
            .sampleEvery(Milliseconds(500))
            .noise(1.6)
            .drift(0.20, 72.0)
            .build());

    profile.sensors.push_back(
        SensorDefinitionBuilder("motor_temp", "Motor Temperature", "C", 58.0)
            .range(0.0, 140.0)
            .nominal(30.0, 75.0)
            .warning(20.0, 90.0)
            .critical(10.0, 105.0)
            .sampleEvery(Milliseconds(500))
            .noise(0.35)
            .drift(0.25, 62.0)
            .model(SensorModel::Thermal)
            .modelParams(4.0, 0.0)
            .build());

    profile.sensors.push_back(
        SensorDefinitionBuilder("water_level", "Water Level", "%", 80.0)
            .range(0.0, 100.0)
            .nominal(60.0, 95.0)
            .warning(45.0, 101.0)
            .critical(25.0, 102.0)
            .sampleEvery(Milliseconds(1000))
            .noise(0.60)
            .drift(0.08, 80.0)
            .build());

    Fault lowPressure;
    lowPressure.sensorId = "water_pressure";
    lowPressure.mode = FaultMode::Offset;
    lowPressure.magnitude = -2.6;
    lowPressure.startMs = 3000;
    lowPressure.durationMs = 5000;
    lowPressure.label = "low_water_pressure";
    profile.faults.push_back(lowPressure);

    Fault lowLevel;
    lowLevel.sensorId = "water_level";
    lowLevel.mode = FaultMode::Offset;
    lowLevel.magnitude = -38.0;
    lowLevel.startMs = 6000;
    lowLevel.durationMs = 4000;
    lowLevel.label = "low_water_level";
    profile.faults.push_back(lowLevel);

    return profile;
}

} // namespace vehiclesim
