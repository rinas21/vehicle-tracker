#include "profiles/MotorProfile.h"

#include "SensorDefinition.h"

namespace vehiclesim {

SystemProfile makeMotorProfile() {
    SystemProfile profile;
    profile.name = "industrial_motor";
    profile.baseSeed = 0x5EED1234u;

    profile.sensors.push_back(
        SensorDefinitionBuilder("motor_temp", "Motor Temperature", "C", 62.0)
            .range(0.0, 160.0)
            .nominal(40.0, 85.0)
            .warning(30.0, 95.0)
            .critical(20.0, 110.0)
            .sampleEvery(Milliseconds(500))
            .noise(0.50)
            .drift(0.25, 70.0)
            .model(SensorModel::Thermal)
            .modelParams(4.0, 0.0)
            .build());

    profile.sensors.push_back(
        SensorDefinitionBuilder("vibration", "Vibration", "mm/s", 2.2)
            .range(0.0, 20.0)
            .nominal(0.5, 4.0)
            .warning(-0.1, 6.0)
            .critical(-0.5, 9.0)
            .sampleEvery(Milliseconds(250))
            .noise(0.35)
            .drift(0.10, 2.2)
            .model(SensorModel::Vibration)
            .modelParams(0.01, 5.0)
            .build());

    profile.sensors.push_back(
        SensorDefinitionBuilder("rotational_speed", "Rotational Speed", "rpm", 1600.0)
            .range(0.0, 4000.0)
            .nominal(1400.0, 1800.0)
            .warning(1200.0, 2200.0)
            .critical(1000.0, 2600.0)
            .sampleEvery(Milliseconds(500))
            .noise(18.0)
            .drift(0.12, 1650.0)
            .build());

    profile.sensors.push_back(
        SensorDefinitionBuilder("current", "Electrical Current", "A", 9.2)
            .range(0.0, 30.0)
            .nominal(6.0, 12.0)
            .warning(4.0, 16.0)
            .critical(2.0, 20.0)
            .sampleEvery(Milliseconds(500))
            .noise(0.30)
            .drift(0.20, 9.5)
            .build());

    Fault vibrationFault;
    vibrationFault.sensorId = "vibration";
    vibrationFault.mode = FaultMode::Offset;
    vibrationFault.magnitude = 6.0;
    vibrationFault.startMs = 2500;
    vibrationFault.durationMs = 4000;
    vibrationFault.label = "excessive_vibration";
    profile.faults.push_back(vibrationFault);

    Fault overcurrent;
    overcurrent.sensorId = "current";
    overcurrent.mode = FaultMode::Scale;
    overcurrent.magnitude = 1.35;
    overcurrent.startMs = 7000;
    overcurrent.durationMs = 4000;
    overcurrent.label = "overcurrent";
    profile.faults.push_back(overcurrent);

    return profile;
}

} // namespace vehiclesim
