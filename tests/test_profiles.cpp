#include <gtest/gtest.h>

#include "PhysicalSystem.h"
#include "SensorDefinition.h"
#include "profiles/ProfileRegistry.h"

#include <set>
#include <stdexcept>
#include <string>

using namespace vehiclesim;

namespace {

void expectValidProfile(const std::string& name, std::size_t expectedSensorCount) {
    const SystemProfile profile = makeProfile(name);

    EXPECT_EQ(profile.name.empty(), false);
    ASSERT_EQ(profile.sensors.size(), expectedSensorCount);
    EXPECT_FALSE(profile.faults.empty());

    std::set<std::string> ids;
    for (const SensorDefinition& definition : profile.sensors) {
        EXPECT_NO_THROW(validateSensorDefinition(definition));
        EXPECT_TRUE(ids.insert(definition.id).second);
    }

    const PhysicalSystem system(profile);
    EXPECT_EQ(system.sensors().size(), expectedSensorCount);
}

} // namespace

TEST(ProfileTests, VehicleProfileIsComplete) {
    expectValidProfile("vehicle", 4);
}

TEST(ProfileTests, MotorProfileIsComplete) {
    expectValidProfile("industrial_motor", 4);
}

TEST(ProfileTests, PumpProfileIsComplete) {
    expectValidProfile("water_pump", 4);
}

TEST(ProfileTests, RegistrySupportsShortAliases) {
    EXPECT_EQ(makeProfile("motor").name, "industrial_motor");
    EXPECT_EQ(makeProfile("pump").name, "water_pump");
}

TEST(ProfileTests, RegistryRejectsUnknownProfile) {
    EXPECT_THROW(makeProfile("unknown"), std::invalid_argument);
}

TEST(ProfileTests, RegistryListsThreeProfiles) {
    EXPECT_EQ(availableProfileNames().size(), 3u);
}
