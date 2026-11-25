# Generic Physical System Monitoring and Diagnostics Simulator

A C++14 console application that simulates physical systems using software-generated
sensor data. It models normal operating behavior, configurable abnormal conditions,
diagnostics, fault injection, concurrent sensor sampling, and CSV logging.

> **Important:** This is a software simulator only. It does not use real sensors,
> microcontrollers, CAN/OBD-II hardware, Arduino, Raspberry Pi, or any external device.
> No claim of real hardware integration is made anywhere in this project.

---

## Features

- C++14, standard library only (plus GoogleTest for unit tests)
- Three simulation profiles:
  - `vehicle`: engine temperature, vehicle speed, battery voltage, oil pressure
  - `industrial_motor`: motor temperature, vibration, rotational speed, electrical current
  - `water_pump`: water pressure, flow rate, motor temperature, water level
- Realistic changing values: bounded drift toward a nominal target plus controlled noise
- Specialized sensor models:
  - generic bounded random walk
  - thermal inertia model
  - vibration spike model
- Rule-based diagnostics: `OK`, `WARNING`, `CRITICAL`
- Time-based fault injection with offset and scale modes
- One `std::thread` per sensor plus one consumer thread
- `std::mutex` + `std::condition_variable` thread-safe queue
- Cooperative shutdown using `std::atomic<bool>`
- CSV export of readings, severities, diagnosis messages, and active faults
- Reproducible runs via fixed or user-supplied seeds
- Unit tests with GoogleTest and GitHub Actions CI

---

## Architecture

```text
SystemProfile (immutable configuration)
  |-- SensorDefinition[]  (ranges, thresholds, period, noise, drift)
  `-- Fault[]             (sensor id, mode, magnitude, start, duration)

PhysicalSystem (runtime instance)
  `-- unique_ptr<ISensor>[]   (one simulated sensor object per definition)

Sensor threads
  |-- SimulatedSensor      generic bounded random walk
  |-- ThermalSensor        exponential response toward thermal target
  `-- VibrationSensor      random walk plus occasional spikes
        |
        v
ThreadSafeQueue<SensorReading>
        |
        v
Simulator consumer thread
        |-- DiagnosticEngine  -> OK / WARNING / CRITICAL
        |-- CsvLogger         -> timestamped CSV row
        `-- Console           -> compact status and alerts
```

### Responsibilities

| Class | Responsibility |
|---|---|
| `SystemProfile` | Holds configuration data; no behavior and no runtime state |
| `SensorDefinition` | Describes one sensor's ranges, thresholds, timing, and model |
| `ISensor` | Abstract interface for any simulated sensor |
| `SimulatedSensor` | Reusable sensor model with seeded randomness and fault application |
| `ThermalSensor` | Adds exponential thermal response via inheritance |
| `VibrationSensor` | Adds probabilistic spikes via inheritance |
| `PhysicalSystem` | Owns sensor objects created from a profile |
| `DiagnosticEngine` | Pure evaluation logic; knows no profile names |
| `FaultInjector` | Converts a time-based fault schedule into a `FaultEffect` |
| `ThreadSafeQueue` | Moves readings safely from producers to one consumer |
| `CsvLogger` | Owns an `std::ofstream` and writes formatted rows |
| `Simulator` | Starts/stops threads and coordinates the full pipeline |

---

## Folder structure

```text
vehicle-tracker/
├── CMakeLists.txt
├── README.md
├── .github/workflows/ci.yml
├── .gitignore
├── include/vehiclesim/
│   ├── CoreTypes.h
│   ├── ISensor.h
│   ├── SensorDefinition.h
│   ├── SystemProfile.h
│   ├── PhysicalSystem.h
│   ├── DiagnosticEngine.h
│   ├── FaultInjector.h
│   ├── ThreadSafeQueue.h
│   ├── CsvLogger.h
│   ├── Simulator.h
│   ├── sensors/
│   │   ├── SimulatedSensor.h
│   │   ├── ThermalSensor.h
│   │   └── VibrationSensor.h
│   └── profiles/
│       ├── VehicleProfile.h
│       ├── MotorProfile.h
│       ├── PumpProfile.h
│       └── ProfileRegistry.h
├── src/
│   ├── main.cpp
│   ├── CoreTypes.cpp
│   ├── SensorDefinition.cpp
│   ├── DiagnosticEngine.cpp
│   ├── FaultInjector.cpp
│   ├── CsvLogger.cpp
│   ├── PhysicalSystem.cpp
│   ├── Simulator.cpp
│   ├── sensors/
│   │   └── ...
│   └── profiles/
│       └── ...
└── tests/
    ├── CMakeLists.txt
    ├── test_sensors.cpp
    ├── test_diagnostics.cpp
    ├── test_faults.cpp
    ├── test_csv.cpp
    ├── test_queue.cpp
    └── test_profiles.cpp
```

---

## Build and test

Requires CMake 3.14+ and a C++14 compiler.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

To build without tests:

```bash
cmake -S . -B build -DBUILD_TESTING=OFF
cmake --build build --parallel
```

GoogleTest v1.14.0 is fetched automatically by CMake with a pinned URL hash.

---

## Run

Show CLI help:

```bash
./build/physical_sim_app --help
```

Vehicle profile:

```bash
./build/physical_sim_app --profile vehicle --duration 12
```

Industrial motor profile:

```bash
./build/physical_sim_app --profile industrial_motor --duration 12
```

Water pump profile:

```bash
./build/physical_sim_app --profile water_pump --duration 12
```

Reproducible run with a fixed seed and no faults:

```bash
./build/physical_sim_app --profile vehicle --duration 10 --seed 12345 --no-faults
```

Custom CSV path and quiet console:

```bash
./build/physical_sim_app --profile pump --duration 8 --csv run.csv --quiet
```

Equivalent `--option=value` form is also supported:

```bash
./build/physical_sim_app --profile=vehicle --duration=5 --csv=run.csv
```

---

## CSV format

One row is written per sensor sample:

```text
timestamp_ms,system,sensor_id,sensor_label,value,unit,severity,diagnosis,active_faults
```

Example:

```text
3000,vehicle,engine_temp,Engine Temperature,119.09,C,CRITICAL,Engine Temperature critically high: 119.09 C,engine_overheat
```

Fields containing commas, quotes, or newlines are escaped with standard CSV quoting.
Multiple active faults are joined with `;` in `active_faults`.

---

## Fault injection

Faults are generic and data-driven:

```text
Fault {
  sensorId,
  mode: Offset or Scale,
  magnitude,
  startMs,
  durationMs,
  label
}
```

Examples in the shipped profiles:

| Profile | Fault | Effect |
|---|---|---|
| Vehicle | `engine_overheat` | +28 °C offset on engine temperature |
| Vehicle | `low_oil_pressure` | -45 psi offset on oil pressure |
| Vehicle | `low_battery_voltage` | -2.4 V offset on battery voltage |
| Motor | `excessive_vibration` | +6 mm/s offset on vibration |
| Motor | `overcurrent` | 1.35x scale on electrical current |
| Pump | `low_water_pressure` | -2.6 bar offset on water pressure |
| Pump | `low_water_level` | -38 % offset on water level |

Faults are applied to the **reported** value. The underlying simulated physical state
continues its normal model, so faults are reversible and easy to test.

---

## Simulation model

Each sensor advances by:

```text
next = current
     + driftRate * dt * (nominalTarget - current)
     + bounded random noise
```

Then fault offset/scale is applied and the result is clamped to the configured physical
range. `ThermalSensor` uses an exponential response instead of linear drift;
`VibrationSensor` can add rare spikes.

Sampling is configurable per sensor via `SensorDefinition::samplePeriod`. Each sensor
thread sleeps in small slices so shutdown remains responsive. This is simulation time,
not a hard real-time guarantee.

---

## Threading and synchronization

- `Simulator` owns one `std::thread` per sensor.
- Each sensor has its own RNG seeded from `profile.baseSeed`, the sensor id, and its index.
  This avoids shared RNG state and therefore avoids a data race.
- Producers push `SensorReading` values into `ThreadSafeQueue`.
- One consumer thread pops readings, diagnoses them, logs them, and prints alerts.
- `ThreadSafeQueue` uses `std::mutex` for the queue and `std::condition_variable` to
  wait without busy-spinning.
- `std::atomic<bool> stopRequested_` lets all threads stop cooperatively.
- After the duration expires, sensor threads are joined, the queue is closed, the
  consumer drains remaining rows, and the logger flushes.

A `Simulator` instance is intended for one `run()` call.

---

## Interview notes

Key points you should be able to explain:

1. **Why an abstract `ISensor`?**
   It decouples the simulation engine from specific sensor models. `PhysicalSystem`
   owns `std::unique_ptr<ISensor>` so sensors are polymorphic and safely destroyed.

2. **Why is `ISensor` destructor virtual?**
   Deleting a derived object through an `ISensor*` must call the derived destructor.
   Without a virtual destructor, cleanup would be undefined behavior.

3. **Why pass `const SystemProfile&`?**
   Factories only read configuration. A const reference avoids a copy and prevents
   accidental modification.

4. **Why return `SensorReading` by value?**
   It is a small value object. Returning by value is simple and safe; move semantics
   avoid expensive copies when pushing into the queue.

5. **Why is `DiagnosticEngine` separate?**
   Diagnostics are reusable across profiles and independently testable. Thresholds
   live in the profile, not in the engine.

6. **Why one consumer thread?**
   A single pipeline guarantees each reading is diagnosed once, logged once, and
   printed once, with no ordering ambiguity between alerts and CSV rows.

7. **Why seeded randomness?**
   Tests can reproduce exact sequences, which is essential for reliable unit tests.

8. **What is not implemented?**
   No real hardware, no CAN/OBD-II, no microcontroller, no hard real-time scheduler,
   no GUI, and no configuration file parser. These are deliberate scope limits.

---

## Limitations

- Values are mathematical approximations, not validated automotive or industrial models.
- Alert thresholds are illustrative, not safety-certified limits.
- Wall-clock sleeps are used; this is not a real-time system.
- Each run simulates one profile at a time.
- Configuration is C++ code rather than external JSON/XML for simplicity.

---

## Milestone history

1. Foundation, CMake, one engine-temperature sensor, single-threaded console output
2. Full vehicle profile, diagnostics, sensor subclasses, GoogleTest
3. CSV logging and generic fault injection
4. Threading, `ThreadSafeQueue`, `Simulator`, atomic shutdown
5. Motor and pump profiles, registry, full CLI
6. GitHub Actions CI, README, final verification
