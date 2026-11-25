#include <gtest/gtest.h>

#include "CsvLogger.h"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

using namespace vehiclesim;

TEST(CsvTests, WritesHeaderAndReadingRow) {
    const std::string path = "test_csv_output.csv";
    std::remove(path.c_str());

    {
        CsvLogger logger(path);
        ASSERT_TRUE(logger.isOpen());
        logger.writeHeader();

        SensorReading reading;
        reading.timestampMs = 1500;
        reading.systemName = "vehicle";
        reading.sensorId = "engine_temp";
        reading.sensorLabel = "Engine Temperature";
        reading.value = 118.25;
        reading.unit = "C";
        reading.severity = Severity::Critical;
        reading.activeFaults.push_back("engine_overheat");

        Diagnosis diagnosis;
        diagnosis.severity = Severity::Critical;
        diagnosis.message = "Engine Temperature critically high: 118.25 C";

        logger.writeReading(reading, diagnosis);
        logger.flush();
    }

    std::ifstream input(path.c_str());
    ASSERT_TRUE(input.is_open());

    std::string header;
    std::string row;
    std::getline(input, header);
    std::getline(input, row);

    EXPECT_NE(header.find("timestamp_ms"), std::string::npos);
    EXPECT_NE(header.find("active_faults"), std::string::npos);
    EXPECT_NE(row.find("engine_temp"), std::string::npos);
    EXPECT_NE(row.find("CRITICAL"), std::string::npos);
    EXPECT_NE(row.find("engine_overheat"), std::string::npos);

    std::remove(path.c_str());
}

TEST(CsvTests, QuotesFieldsContainingCommas) {
    const std::string path = "test_csv_quotes.csv";
    std::remove(path.c_str());

    {
        CsvLogger logger(path);
        SensorReading reading;
        reading.systemName = "pump, main";
        reading.sensorId = "pressure";
        reading.sensorLabel = "Pressure";
        reading.unit = "bar";
        reading.value = 4.2;

        Diagnosis diagnosis;
        diagnosis.message = "Normal, stable";
        logger.writeReading(reading, diagnosis);
    }

    std::ifstream input(path.c_str());
    std::string row;
    std::getline(input, row);

    EXPECT_NE(row.find("\"pump, main\""), std::string::npos);
    EXPECT_NE(row.find("\"Normal, stable\""), std::string::npos);

    std::remove(path.c_str());
}
