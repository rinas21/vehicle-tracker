#ifndef VEHICLESIM_CSVLOGGER_H
#define VEHICLESIM_CSVLOGGER_H

#include "CoreTypes.h"

#include <fstream>
#include <string>

namespace vehiclesim {

class CsvLogger {
public:
    explicit CsvLogger(std::string path);
    ~CsvLogger();

    CsvLogger(const CsvLogger&) = delete;
    CsvLogger& operator=(const CsvLogger&) = delete;

    bool isOpen() const;
    void writeHeader();
    void writeReading(const SensorReading& reading, const Diagnosis& diagnosis);
    void flush();

private:
    std::string path_;
    std::ofstream stream_;
};

} // namespace vehiclesim

#endif // VEHICLESIM_CSVLOGGER_H
