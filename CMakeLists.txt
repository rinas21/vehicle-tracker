cmake_minimum_required(VERSION 3.14)
project(PhysicalSystemSimulator LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTING "Build unit tests" ON)
option(ENABLE_WARNINGS "Enable compiler warnings" ON)

set(SIMULATOR_SOURCES
    src/CoreTypes.cpp
    src/SensorDefinition.cpp
    src/DiagnosticEngine.cpp
    src/FaultInjector.cpp
    src/CsvLogger.cpp
    src/PhysicalSystem.cpp
    src/Simulator.cpp
    src/sensors/SimulatedSensor.cpp
    src/sensors/ThermalSensor.cpp
    src/sensors/VibrationSensor.cpp
    src/profiles/VehicleProfile.cpp
    src/profiles/MotorProfile.cpp
    src/profiles/PumpProfile.cpp
    src/profiles/ProfileRegistry.cpp
)

add_library(physical_sim STATIC ${SIMULATOR_SOURCES})

target_include_directories(physical_sim PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vehiclesim
)

if(ENABLE_WARNINGS)
    target_compile_options(physical_sim PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(physical_sim_app src/main.cpp)
target_link_libraries(physical_sim_app PRIVATE physical_sim)

if(ENABLE_WARNINGS)
    target_compile_options(physical_sim_app PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
