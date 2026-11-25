#include "Simulator.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <utility>

namespace vehiclesim {

Simulator::Simulator(SystemProfile profile, std::string csvPath, bool printConsole)
    : system_(std::move(profile)),
      faultInjector_(system_.profile().faults),
      diagnosticEngine_(),
      logger_(std::move(csvPath)),
      queue_(),
      stopRequested_(false),
      running_(false),
      printConsole_(printConsole) {}

Simulator::~Simulator() {
    requestStop();
    stopSensorThreads();
    queue_.close();
    if (consumerThread_.joinable()) {
        consumerThread_.join();
    }
}

void Simulator::requestStop() {
    stopRequested_.store(true);
}

bool Simulator::isRunning() const {
    return running_.load();
}

const SensorDefinition* Simulator::findDefinition(const std::string& sensorId) const {
    for (const SensorDefinition& definition : system_.profile().sensors) {
        if (definition.id == sensorId) {
            return &definition;
        }
    }
    return nullptr;
}

void Simulator::stopSensorThreads() {
    for (std::thread& thread : sensorThreads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void Simulator::sensorLoop(ISensor* sensor, Milliseconds period,
                           SimulationTime maxDurationMs) {
    SimulationTime elapsedMs = 0;

    while (!stopRequested_.load()) {
        if (maxDurationMs > 0 && elapsedMs >= maxDurationMs) {
            break;
        }

        const FaultEffect effect = faultInjector_.effectFor(sensor->id(), elapsedMs);
        SensorReading reading = sensor->sample(elapsedMs, period, effect);
        reading.activeFaults = faultInjector_.activeLabelsFor(sensor->id(), elapsedMs);
        queue_.push(std::move(reading));

        const int sliceMs = 20;
        int sleptMs = 0;
        while (sleptMs < period.count() && !stopRequested_.load()) {
            const int remainingMs = static_cast<int>(period.count()) - sleptMs;
            const int stepMs = std::min(sliceMs, remainingMs);
            std::this_thread::sleep_for(Milliseconds(stepMs));
            sleptMs += stepMs;
        }

        elapsedMs += static_cast<SimulationTime>(period.count());
    }
}

void Simulator::consumerLoop() {
    SensorReading reading;

    while (queue_.waitPop(reading)) {
        const SensorDefinition* definition = findDefinition(reading.sensorId);

        Diagnosis diagnosis;
        if (definition != nullptr) {
            diagnosis = diagnosticEngine_.evaluate(*definition, reading);
        }
        reading.severity = diagnosis.severity;

        logger_.writeReading(reading, diagnosis);

        if (printConsole_) {
            if (reading.severity == Severity::Ok) {
                std::cout << "t=" << reading.timestampMs << "ms  " << reading.sensorLabel
                          << " = " << reading.value << " " << reading.unit
                          << "  [OK]\n";
            } else {
                std::cout << "t=" << reading.timestampMs << "ms  [ALERT] "
                          << diagnosis.message;
                if (!reading.activeFaults.empty()) {
                    std::cout << "  (injected: " << reading.activeFaults.front() << ")";
                }
                std::cout << '\n';
            }
        }
    }
}

void Simulator::run(Milliseconds duration) {
    bool expected = false;
    if (!running_.compare_exchange_strong(expected, true)) {
        return;
    }

    if (!logger_.isOpen()) {
        running_.store(false);
        return;
    }

    logger_.writeHeader();
    stopRequested_.store(false);

    if (printConsole_) {
        std::cout << "Simulation started with " << system_.sensors().size()
                  << " sensor threads and 1 consumer thread.\n";
        std::cout << "Software simulation only; no hardware is used.\n\n";
    }

    consumerThread_ = std::thread(&Simulator::consumerLoop, this);

    const SimulationTime maxDurationMs = static_cast<SimulationTime>(duration.count());
    for (std::size_t index = 0; index < system_.sensors().size(); ++index) {
        ISensor* sensor = system_.sensors()[index].get();
        const Milliseconds period = system_.profile().sensors[index].samplePeriod;
        sensorThreads_.emplace_back(&Simulator::sensorLoop, this, sensor, period,
                                    maxDurationMs);
    }

    const auto deadline = std::chrono::steady_clock::now() + duration;
    while (!stopRequested_.load() &&
           (maxDurationMs <= 0 || std::chrono::steady_clock::now() < deadline)) {
        std::this_thread::sleep_for(Milliseconds(20));
    }

    stopRequested_.store(true);
    stopSensorThreads();
    queue_.close();

    if (consumerThread_.joinable()) {
        consumerThread_.join();
    }

    logger_.flush();
    running_.store(false);

    if (printConsole_) {
        std::cout << "\nSimulation finished.\n";
    }
}

} // namespace vehiclesim
