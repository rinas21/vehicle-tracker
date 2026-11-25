#include "profiles/VehicleProfile.h"

#include "SensorDefinition.h"

namespace vehiclesim {

SystemProfile makeVehicleProfile() {
    SystemProfile profile;
    profile.name = "vehicle";
    profile.baseSeed = 0xC0FFEEu;

    profile.sensors.push_back(
        SensorDefinitionBuilder("engine_temp", "Engine Temperature", "C", 88.0)
            .range(20.0, 140.0)
            .nominal(80.0, 105.0)
            .warning(70.0, 106.0)
            .critical(60.0, 116.0)
            .sampleEvery(Milliseconds(500))
            .noise(0.40)
            .drift(0.35, 92.0)
            .model(SensorModel::Thermal)
            .modelParams(3.0, 0.0)
            .build());

    profile.sensors.push_back(
        SensorDefinitionBuilder("vehicle_speed", "Vehicle Speed", "km/h", 20.0)
            .range(0.0, 260.0)
            .nominal(0.0, 180.0)
            .warning(-1.0, 190.0)
            .critical(-5.0, 220.0)
            .sampleEvery(Milliseconds(500))
            .noise(1.8)
            .drift(0.08, 90.0)
            .build());

    profile.sensors.push_back(
        SensorDefinitionBuilder("battery_voltage", "Battery Voltage", "V", 14.1)
            .range(10.0, 18.0)
            .nominal(12.4, 14.8)
            .warning(12.0, 15.2)
            .critical(11.5, 16.0)
            .sampleEvery(Milliseconds(500))
            .noise(0.08)
            .drift(0.30, 14.1)
            .build());

    profile.sensors.push_back(
        SensorDefinitionBuilder("oil_pressure", "Oil Pressure", "psi", 52.0)
            .range(0.0, 120.0)
            .nominal(30.0, 80.0)
            .warning(25.0, 85.0)
            .critical(15.0, 95.0)
            .sampleEvery(Milliseconds(500))
            .noise(1.2)
            .drift(0.25, 55.0)
            .build());

    Fault overheat;
    overheat.sensorId = "engine_temp";
    overheat.mode = FaultMode::Offset;
    overheat.magnitude = 28.0;
    overheat.startMs = 3000;
    overheat.durationMs = 5000;
    overheat.label = "engine_overheat";
    profile.faults.push_back(overheat);

    Fault lowOilPressure;
    lowOilPressure.sensorId = "oil_pressure";
    lowOilPressure.mode = FaultMode::Offset;
    lowOilPressure.magnitude = -45.0;
    lowOilPressure.startMs = 6000;
    lowOilPressure.durationMs = 4000;
    lowOilPressure.label = "low_oil_pressure";
    profile.faults.push_back(lowOilPressure);

    Fault lowVoltage;
    lowVoltage.sensorId = "battery_voltage";
    lowVoltage.mode = FaultMode::Offset;
    lowVoltage.magnitude = -2.4;
    lowVoltage.startMs = 9000;
    lowVoltage.durationMs = 3000;
    lowVoltage.label = "low_battery_voltage";
    profile.faults.push_back(lowVoltage);

    return profile;
}

} // namespace vehiclesim
