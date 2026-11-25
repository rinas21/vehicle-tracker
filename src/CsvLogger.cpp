#include "CsvLogger.h"

#include <sstream>
#include <utility>

namespace vehiclesim {

namespace {

std::string escapeCsv(const std::string& value) {
    const bool needsQuoting = value.find_first_of(",\"\n\r") != std::string::npos;
    if (!needsQuoting) {
        return value;
    }

    std::string escaped;
    escaped.reserve(value.size() + 2);
    escaped.push_back('"');
    for (char character : value) {
        if (character == '"') {
            escaped += "\"\"";
        } else {
            escaped.push_back(character);
        }
    }
    escaped.push_back('"');
    return escaped;
}

std::string joinFaults(const std::vector<std::string>& faults) {
    std::ostringstream stream;
    for (std::size_t index = 0; index < faults.size(); ++index) {
        if (index != 0) {
            stream << ';';
        }
        stream << faults[index];
    }
    return stream.str();
}

} // namespace

CsvLogger::CsvLogger(std::string path)
    : path_(std::move(path)),
      stream_(path_.c_str(), std::ios::out | std::ios::trunc) {}

CsvLogger::~CsvLogger() {
    flush();
}

bool CsvLogger::isOpen() const {
    return stream_.is_open();
}

void CsvLogger::writeHeader() {
    if (!isOpen()) {
        return;
    }

    stream_ << "timestamp_ms,system,sensor_id,sensor_label,value,unit,severity,"
               "diagnosis,active_faults\n";
}

void CsvLogger::writeReading(const SensorReading& reading, const Diagnosis& diagnosis) {
    if (!isOpen()) {
        return;
    }

    stream_ << reading.timestampMs << ','
            << escapeCsv(reading.systemName) << ','
            << escapeCsv(reading.sensorId) << ','
            << escapeCsv(reading.sensorLabel) << ','
            << reading.value << ','
            << escapeCsv(reading.unit) << ','
            << toString(reading.severity) << ','
            << escapeCsv(diagnosis.message) << ','
            << escapeCsv(joinFaults(reading.activeFaults)) << '\n';
}

void CsvLogger::flush() {
    if (stream_.is_open()) {
        stream_.flush();
    }
}

} // namespace vehiclesim
