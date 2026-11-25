#ifndef VEHICLESIM_SIMULATOR_H
#define VEHICLESIM_SIMULATOR_H

#include "CoreTypes.h"
#include "CsvLogger.h"
#include "DiagnosticEngine.h"
#include "FaultInjector.h"
#include "ISensor.h"
#include "PhysicalSystem.h"
#include "SystemProfile.h"
#include "ThreadSafeQueue.h"

#include <atomic>
#include <string>
#include <thread>
#include <vector>

namespace vehiclesim {

class Simulator {
public:
    Simulator(SystemProfile profile, std::string csvPath, bool printConsole);
    ~Simulator();

    Simulator(const Simulator&) = delete;
    Simulator& operator=(const Simulator&) = delete;

    void run(Milliseconds duration);
    void requestStop();
    bool isRunning() const;

private:
    PhysicalSystem system_;
    FaultInjector faultInjector_;
    DiagnosticEngine diagnosticEngine_;
    CsvLogger logger_;
    ThreadSafeQueue<SensorReading> queue_;
    std::atomic<bool> stopRequested_;
    std::atomic<bool> running_;
    bool printConsole_;
    std::vector<std::thread> sensorThreads_;
    std::thread consumerThread_;

    void sensorLoop(ISensor* sensor, Milliseconds period, SimulationTime maxDurationMs);
    void consumerLoop();
    const SensorDefinition* findDefinition(const std::string& sensorId) const;
    void stopSensorThreads();
};

} // namespace vehiclesim

#endif // VEHICLESIM_SIMULATOR_H
